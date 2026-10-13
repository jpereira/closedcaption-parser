#!/usr/bin/env python3
"""
Gera os arquivos MPEG-TS sintéticos usados pelo "make test".

  sintetico-basico.ts        legendas simples: acentos, CS/APR, PES em vários
                             pacotes TS, cabeçalho com 15 bytes extras e uma
                             legenda one-seg que não deve aparecer.
  sintetico-casos-limite.ts  comandos não suportados com parâmetros, ESC, CSI
                             desconhecido, til, SS3 (♪), SZX, legenda com mais
                             de 255 bytes, DRCS antes do texto, CRC inválido,
                             stuffing e comandos truncados.

Uso: python3 tests/gerar-sinteticos.py [diretório de saída]
"""
import os
import sys


def crc32_mpeg(data):
    """CRC-32/MPEG-2 das seções PSI (PAT/PMT)."""
    crc = 0xffffffff
    for b in data:
        crc ^= b << 24
        for _ in range(8):
            crc = ((crc << 1) ^ 0x04C11DB7) if crc & 0x80000000 else crc << 1
            crc &= 0xffffffff
    return crc


def crc16_ccitt(data):
    """CRC-16/CCITT do data_group da legenda (ARIB STD-B24)."""
    crc = 0
    for b in data:
        crc ^= b << 8
        for _ in range(8):
            crc = ((crc << 1) ^ 0x1021) & 0xffff if crc & 0x8000 else (crc << 1) & 0xffff
    return crc


class Mux:
    """Gera pacotes TS de 188 bytes, controlando o continuity_counter por PID."""

    def __init__(self):
        self.cc = {}
        self.out = b''

    def put(self, pid, payload):
        first = True
        while payload or first:
            chunk, payload = payload[:184], payload[184:]
            cc = self.cc.get(pid, 0)
            self.cc[pid] = (cc + 1) & 15
            hdr = bytes([0x47, (0x40 if first else 0) | (pid >> 8), pid & 0xff])
            if len(chunk) < 184:
                # adaptation_field com stuffing para completar os 188 bytes
                st = 184 - len(chunk) - 1
                af = bytes([st]) + (bytes([0x00]) + b'\xff' * (st - 1) if st > 0 else b'')
                self.out += hdr + bytes([0x30 | cc]) + af + chunk
            else:
                self.out += hdr + bytes([0x10 | cc]) + chunk
            first = False


def section(table_id, ext, body):
    length = len(body) + 5 + 4
    s = bytes([table_id, 0xb0 | (length >> 8), length & 0xff, ext >> 8, ext & 0xff, 0xc1, 0, 0]) + body
    return b'\x00' + s + crc32_mpeg(s).to_bytes(4, 'big')  # pointer_field + seção


def pts_bytes(p):
    return bytes([0x21 | ((p >> 29) & 0x0e), (p >> 22) & 0xff, 0x01 | ((p >> 14) & 0xfe),
                  (p >> 7) & 0xff, 0x01 | ((p << 1) & 0xfe)])


def data_group(group_id, body, crc_ok=True):
    g = bytes([group_id << 2, 0, 0, len(body) >> 8, len(body) & 0xff]) + body
    crc = crc16_ccitt(g) ^ (0 if crc_ok else 0xffff)
    return g + crc.to_bytes(2, 'big')


def management():
    # TMD=00, 1 idioma "por", DMF=0, formato/TCS/rollup=0, sem data units
    return data_group(0, bytes([0x3f, 1, 0x00]) + b'por' + bytes([0x00]) + bytes([0, 0, 0]))


def statement(*units):
    """units: lista de (data_unit_parameter, bytes)."""
    loop = b''
    for param, data in units:
        loop += bytes([0x1f, param]) + len(data).to_bytes(3, 'big') + data
    return bytes([0x3f]) + len(loop).to_bytes(3, 'big') + loop


def pes(pts, group, extra_header=0):
    body = bytes([0x80, 0xff, 0xf0 | extra_header]) + b'\xff' * extra_header + group
    h = bytes([0x81, 0x80, 5]) + pts_bytes(pts) + body
    return b'\x00\x00\x01\xbd' + len(h).to_bytes(2, 'big') + h


def latin(s):
    return s.encode('iso-8859-15')


def basico():
    m = Mux()
    pat = section(0, 1, bytes([0, 1, 0xe1, 0x00]))
    es = bytes([0x1b, 0xe1, 0x11, 0xf0, 3, 0x52, 1, 0x00])   # vídeo
    es += bytes([0x06, 0xe1, 0x38, 0xf0, 3, 0x52, 1, 0x87])  # legenda one-seg
    es += bytes([0x06, 0xe1, 0x30, 0xf0, 3, 0x52, 1, 0x30])  # legenda full-seg
    pmt = section(2, 1, bytes([0xe1, 0x11, 0xf0, 0]) + es)

    caps = [
        (90000 * 10, latin('Olá, mundo!')),
        (90000 * 12 + 45000, b'\x0c\x87' + latin('Primeira linha') + b'\x0d' + latin('Segunda linha com ação')),
        (90000 * 15, b'\x0c' + latin('Uma legenda bem comprida para ocupar mais de um pacote TS, ')
         + latin('testando a remontagem do PES no exemplo, ') + latin('com acentuação: é, ç, ã, õ, ü.')
         + b'\x0d' + latin('Fim.')),
    ]

    m.put(0x0000, pat)
    m.put(0x0100, pmt)
    m.put(0x0130, pes(90000 * 9, management()))
    for i, (p, text) in enumerate(caps):
        m.put(0x0111, b'\x00\x00\x01\xe0\x00\x00' + b'\x00' * 50)
        m.put(0x0130, pes(p, data_group(1, statement((0x20, text))), 15 if i == 1 else 0))
        m.put(0x0138, pes(p, data_group(1, statement((0x20, latin('ONE-SEG NAO DEVE APARECER'))))))
    return m.out


def casos_limite():
    m = Mux()
    pat = section(0, 1, bytes([0, 1, 0xe1, 0x00]))
    pmt = section(2, 1, bytes([0xe1, 0x30, 0xf0, 0]) + bytes([0x06, 0xe1, 0x30, 0xf0, 3, 0x52, 1, 0x30]))

    groups = [
        # FLC, HLC, TIME, ESC e CSI desconhecido: os parâmetros não podem virar texto
        data_group(1, statement((0x20, latin('A') + b'\x91\x40' + latin('B') + b'\x97\x41' + latin('C')
                                 + b'\x9d\x20\x45' + latin('D') + b'\x1b\x28\x4a' + latin('E')
                                 + b'\x9b\x31\x20\x54' + latin('F')))),
        # til, SS3 (nota musical) e SZX
        data_group(1, statement((0x20, latin('til~ ') + b'\x1d\x21' + latin(' musica') + b'\x8b\x60' + latin('!')))),
        # legenda com mais de 255 bytes
        data_group(1, statement((0x20, latin('X' * 150) + b'\x0d' + latin('Y' * 150) + latin('FIM')))),
        # DRCS antes do texto
        data_group(1, statement((0x30, b'\x01\x02\x03\x04'), (0x20, latin('Depois do DRCS')))),
        # CRC inválido: deve ser descartado
        data_group(1, statement((0x20, latin('NAO DEVE APARECER (CRC)'))), crc_ok=False),
        # stuffing depois do data_group
        data_group(1, statement((0x20, latin('Com stuffing')))) + b'\xff' * 40,
        # comandos truncados no final do texto: APS, COL e CSI sem o byte final
        data_group(1, statement((0x20, latin('Truncado') + b'\x1c\x41'))),
        data_group(1, statement((0x20, latin('Segundo truncado') + b'\x90'))),
        data_group(1, statement((0x20, latin('Terceiro truncado') + b'\x9b\x31\x32'))),
        # CS e nova legenda
        data_group(1, statement((0x20, b'\x0c' + latin('Ultima')))),
    ]

    m.put(0x0000, pat)
    m.put(0x0100, pmt)
    for i, g in enumerate(groups):
        m.put(0x0130, pes(90000 * (10 + i), g))
    return m.out


def main():
    outdir = sys.argv[1] if len(sys.argv) > 1 else os.path.dirname(os.path.abspath(__file__))
    for name, data in (('sintetico-basico.ts', basico()), ('sintetico-casos-limite.ts', casos_limite())):
        with open(os.path.join(outdir, name), 'wb') as f:
            f.write(data)
        print(f'{name}: {len(data)} bytes')


if __name__ == '__main__':
    main()
