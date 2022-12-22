#!/usr/bin/env python
import os
import zlib
import sys

# running strace -f ./topo_order_commits.py shows that the only execve being called is this python script


class CommitNode:
    def __init__(self, commit_hash: str, branches: list[str] = []):
        self.commit_hash: str = commit_hash
        self.branches: list[str] = branches
        self.parents: set["CommitNode"] = set()
        self.children: set["CommitNode"] = set()

    def __lt__(self, other):
        return isinstance(other, CommitNode) and self.commit_hash < other.commit_hash

    def __hash__(self):
        return hash(self.commit_hash)


# gets the path to the .git directory if it exists
def get_git_path() -> str:
    cur_path = os.getcwd()
    while True:
        git_path = f"{cur_path}/.git"
        if os.path.isdir(git_path):
            cur_path = git_path
            break
        if cur_path == "/":
            print("Not inside a Git repository", file=sys.stderr)
            exit(1)
        cur_path = os.path.abspath(os.path.join(cur_path, os.pardir))
    return cur_path


# gets a dictionary mapping branch hashes to list of corresponding branch names
def get_branches(git_path: str) -> dict[str, list[str]]:
    heads_path = f"{git_path}/refs/heads"
    branch_names = []
    for (dirname, _, files) in os.walk(heads_path):
        branch_names.extend([f"{dirname}/{f}"[len(heads_path) + 1 :] for f in files])
    branches = {}
    for branch_name in branch_names:
        path = f"{heads_path}/{branch_name}"
        branch_hash = open(path, "rb").read().decode("utf-8").strip()
        branches.setdefault(branch_hash, []).append(branch_name)
    return branches


# gets a set of parent hashes of the given commit hash
def get_parent_hashes(git_path: str, commit: str) -> set[str]:
    path = f"{git_path}/objects/{commit[0:2]}/{commit[2:]}"
    raw = open(path, "rb").read()
    decoded = zlib.decompress(raw).decode("utf-8")
    parents = set()
    for line in decoded.split("\n"):
        words = line.split(" ")
        if words[0] == "parent":
            parents.add(words[1])
    return parents


# creates the graph outlined in the spec
def build_graph(
    git_path: str, branches: dict[str, list[str]]
) -> tuple[list[CommitNode], dict[str, CommitNode]]:
    root_commits: list[CommitNode] = []
    commits: dict[str, CommitNode] = {}
    commit_stack: list[CommitNode] = []

    for branch_hash in branches:
        branch = CommitNode(branch_hash)
        commit_stack.append(branch)
        commits[branch_hash] = branch

    while len(commit_stack) > 0:
        cur_node = commit_stack.pop()
        cur_node.branches = branches.get(cur_node.commit_hash, [])
        commits[cur_node.commit_hash] = cur_node
        parent_hashes = get_parent_hashes(git_path, cur_node.commit_hash)
        if len(parent_hashes) == 0:
            root_commits.append(cur_node)
        else:
            parents = []
            for parent_hash in parent_hashes:
                if parent_hash in commits:
                    parent = commits[parent_hash]
                else:
                    parent = CommitNode(parent_hash)
                    parents.append(parent)
                parent.children.add(cur_node)
                cur_node.parents.add(parent)
            commit_stack.extend(parents)
    return root_commits, commits


# topologically sorts the commit graph
def topological_sort(
    root_commits: list[CommitNode], commits: dict[str, CommitNode]
) -> list[CommitNode]:
    sorted_commits: list[CommitNode] = []
    commit_stack: list[CommitNode] = root_commits
    visited_edges: dict[str, list[CommitNode]] = {}
    while len(commit_stack) > 0:
        cur_node = commit_stack.pop()
        sorted_commits.append(cur_node)
        for child in sorted(cur_node.children):
            visited_edges.setdefault(cur_node.commit_hash, []).append(child)
            incoming_edges = sum(
                1
                for commit in commits.values()
                if (child in commit.children)
                and (child not in visited_edges.get(commit.commit_hash, []))
            )
            if incoming_edges == 0:
                commit_stack.append(child)
    return sorted_commits[::-1]


# prints out the graph with sticky end/starts
def print_graph(sorted_commits: list[CommitNode]):
    sticky_start = False
    for i, commit in enumerate(sorted_commits):
        if sticky_start:
            start = " ".join([c.commit_hash for c in commit.children])
            print(f"={start}")
            sticky_start = False
        branch_str = " ".join([str(b) for b in commit.branches])
        print(f"{commit.commit_hash} {branch_str}", end="")
        end = ""
        if i != len(sorted_commits) - 1 and sorted_commits[i + 1] not in commit.parents:
            end = " ".join([c.commit_hash for c in commit.parents])
            end = f"\n{end}=\n"
            sticky_start = True
        print(end)


def topo_order_commits():
    git_path = get_git_path()
    branches = get_branches(git_path)
    root_commits, commits = build_graph(git_path, branches)
    sorted_commits = topological_sort(root_commits, commits)
    print_graph(sorted_commits)


if __name__ == "__main__":
    topo_order_commits()
