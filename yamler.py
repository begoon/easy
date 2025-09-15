import io
from dataclasses import fields, is_dataclass
from typing import Mapping

from ruamel.yaml import YAML

from lexer import Token
from parser import Node


def walker(obj: Node, *, seen: set[int] | None = None) -> Node:
    if seen is None:
        seen = set()

    if obj is None or isinstance(obj, (bool, int, float, str)):
        return obj

    oid = id(obj)
    if oid in seen:
        raise ValueError("cycle detected in AST; serialization would recurse forever.")
    seen.add(oid)

    if is_dataclass(obj):
        if isinstance(obj, Token):
            data = f"<{obj.value}|{obj.type} {obj.input.filename}:{obj.line}:{obj.col}>"
        else:
            data = {}
            data["node"] = obj.__class__.__name__
            for f in fields(obj):
                if f.name.startswith("_"):
                    continue
                value = getattr(obj, f.name)
                data[f.name] = walker(value, seen=seen)
        seen.remove(oid)
        return data

    if isinstance(obj, Mapping):
        out = {}
        for k, v in obj.items():
            key = k if isinstance(k, str) else repr(k)
            out[key] = walker(v, seen=seen)
        seen.remove(oid)
        return out

    if isinstance(obj, (list, tuple, set)):
        seq = [walker(x, seen=seen) for x in obj]
        seen.remove(oid)
        return seq

    try:
        return str(obj)
    finally:
        seen.remove(oid)


def yamlizer(root: Node) -> str:
    data = walker(root)
    yaml = YAML()
    yaml.default_flow_style = False
    yaml.indent(mapping=2, sequence=4, offset=2)
    stream = io.StringIO()
    yaml.dump(data, stream)
    return stream.getvalue()
