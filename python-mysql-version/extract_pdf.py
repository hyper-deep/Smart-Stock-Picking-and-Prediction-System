# -*- coding: utf-8 -*-
"""Extract text from 数据结构课程练习项目-智能股票.pdf (WPS-generated, CID fonts).
Handles both `(...)Tj` literal strings and `<hex>Tj` / [<hex>...] TJ operators.
"""
import zlib, re, sys

PDF = r"C:\Users\fpc_j\Downloads\数据结构课程练习项目-智能股票.pdf"
data = open(PDF, "rb").read()

def decompress(raw):
    if raw.endswith(b"\r\n"):
        raw = raw[:-2]
    elif raw.endswith(b"\n"):
        raw = raw[:-1]
    try:
        return zlib.decompress(raw)
    except Exception:
        return None

streams = []
for m in re.finditer(rb"stream\r?\n(.*?)endstream", data, re.S):
    d = decompress(m.group(1))
    if d is not None:
        streams.append(d)

# ---- build CMap: CID -> unicode (merge all CMap streams) ----
cid2uni = {}
for s in streams:
    if b"begincmap" not in s[:300]:
        continue
    for m in re.finditer(rb"beginbfchar(.*?)endbfchar", s, re.S):
        for e in re.finditer(rb"<([0-9A-Fa-f]+)>\s*<([0-9A-Fa-f]+)>", m.group(1)):
            cid = int(e.group(1), 16)
            hexu = e.group(2)
            if len(hexu) % 4 == 0 and len(hexu) > 4:
                chars = [chr(int(hexu[i:i+4], 16)) for i in range(0, len(hexu), 4)]
                cid2uni[cid] = "".join(chars)
            else:
                cid2uni[cid] = chr(int(hexu, 16))
    for m in re.finditer(rb"beginbfrange(.*?)endbfrange", s, re.S):
        for e in re.finditer(rb"<([0-9A-Fa-f]+)>\s*<([0-9A-Fa-f]+)>\s*<([0-9A-Fa-f]+)>", m.group(1)):
            lo = int(e.group(1), 16); hi = int(e.group(2), 16)
            base = int(e.group(3), 16)
            for off in range(hi - lo + 1):
                cid2uni[lo + off] = chr(base + off)

print("CMap entries:", len(cid2uni), file=sys.stderr)

def decode_cids(byte_seq):
    out = []
    i = 0
    b = bytes(byte_seq)
    while i < len(b) - 1:
        cid = (b[i] << 8) | b[i+1]
        out.append(cid2uni.get(cid, ""))
        i += 2
    return "".join(out)

def decode_hex(h):
    out = []
    for i in range(0, len(h) - 1, 4):
        seg = h[i:i+4]
        if len(seg) == 4:
            out.append(cid2uni.get(int(seg, 16), ""))
    return "".join(out)

# ---- parse content streams ----
results = []
for s in streams:
    if b"Tj" not in s and b"TJ" not in s:
        continue
    # tokenize operators sequentially
    pos = 0
    # pattern for (string)Tj literal - handle escapes: \n \r \t \b \f \( \) \\ \ddd and \xhh
    for m in re.finditer(rb"\(((?:\\.|[^\\()])*)\)\s*Tj|\[([^\]\[\s][^\]]*)\]\s*TJ|<([0-9A-Fa-f]+)>\s*Tj", s):
        if m.group(1) is not None:
            raw = m.group(1)
            # decode escapes
            bytes_out = bytearray()
            i = 0
            while i < len(raw):
                ch = raw[i:i+1]
                if ch == b"\\" and i + 1 < len(raw):
                    nxt = raw[i+1:i+2]
                    mapping = {b"n": 10, b"r": 13, b"t": 9, b"b": 8, b"f": 12, b"(": 40, b")": 41, b"\\": 92}
                    if nxt in mapping:
                        bytes_out.append(mapping[nxt]); i += 2; continue
                    elif re.match(rb"\d", nxt) and i + 3 < len(raw) and raw[i+1:i+4].isdigit():
                        bytes_out.append(int(raw[i+1:i+4]) & 0xFF); i += 4; continue
                    elif nxt == b"x" and i + 3 < len(raw):
                        try:
                            bytes_out.append(int(raw[i+2:i+4], 16)); i += 4; continue
                        except ValueError:
                            pass
                    else:
                        bytes_out.append(nxt[0]); i += 2; continue
                else:
                    bytes_out.append(ch[0]); i += 1
            results.append(decode_cids(bytes_out))
        elif m.group(2) is not None:
            for h in re.finditer(rb"<([0-9A-Fa-f]+)>", m.group(2)):
                results.append(decode_hex(h.group(1)))
        elif m.group(3) is not None:
            results.append(decode_hex(m.group(3)))

out = "".join(results)
out = re.sub(r"[\x00-\x08\x0b\x0c\x0e-\x1f]", "", out)
out = out.encode("utf-8", errors="replace").decode("utf-8", errors="replace")
sys.stdout.reconfigure(encoding="utf-8", errors="replace")
print(out)
