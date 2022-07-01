/**
 * @file: packet-pes.c
 * @autor: Jorge Pereira <jorge.pereira@tqtvd.com>
 * @date: Thu Aug 12 13:49:21 BRT 2010
 * @desc: Mais informações em 'arquitetura-closedcaption.txt'
 */
#include "packet-pes.h"
#include "caption.h"

//#define DEBUG_PACKET_PES

bool
packet_pes_load (const uint8_t *pack_raw,
                 const size_t raw_len,
                 PacketPES_t *out)
{
  PacketPES_t *pes = out;
  const uint8_t *pack = pack_raw;
  uint32_t header = 0;
  
  assert (pack_raw != NULL);
  assert (out != NULL);
  assert (raw_len > 0);
  assert (pes != NULL);

// TODO: Adicionar o esquema dentro de um for() utilizando PacketPES_t::next

  header = ((pack[0] << 16 | pack[1] << 8 | pack[2]) & 0xFFFFFF);
  if (header != PACKET_PES_PREFIX) // ATSC A/90 sync/async streams. (CaptionClosed)
    return FALSE;
  
  // Packet Start Header
  pes->stream_id = pack[3];
  pes->pack_len  = ((pack[4] << 8) | pack[5]);

  // PES Packet header
  pes->header.flags.raw = ((pack[6] << 8) | pack[7]);
  pes->header.len       = pack[8];

  pes->payload.data = NULL;
  pes->payload.len  = 0;

  // +6 completando todo o buf.
  ATV_DEBUG ("Parsing PES Packet, len=%d (0x%04x)", pes->pack_len+6, pes->pack_len+6);
#if 0//DEBUG_PACKET_PES
  ATV_HEXDUMP (pes_raw, raw_len);
#endif

  if (pes->header.flags.bit.emulation && !pes->header.flags.bit.scrambling)
  {
    switch (pes->header.flags.bit.PTS_DTS_flag)
    {
      case PTS_is_FORBIDEN: /* Proibido */
      {
        ATV_DEBUG("The PTS Extension is (PTS_is_FORBIDEN)");
        return FALSE;
      }
      break;

      case PTS_is_ONLY_PTS: /* apenas pacotes PTS (Presentation Time Stamp) */
      {
        if ((pack[9] & 0x20) != 0x20) /* e um PTS valido!? */
        {
          ATV_DEBUG ("** Error: PTS Invalido, saindo...\n");
          return FALSE;
        }
        else
        {
          uint32_t pts = ((pack[9] & 0x0E) << 29) | (pack[10] << 22) | ((pack[11] >> 1) << 15) | (pack[12] << 7) | (pack[13] >> 1);
          uint8_t pos = 0x20; // posicao do 'payload'

          packet_pes_set_pts (&pts, &pes->pts);

          pes->payload.header = (pack[pos] << 16) | (pack[pos+1] << 8) | pack[pos+2];
          pes->payload.data   = &pack[pos + 3];
          pes->payload.len    = (pes->pack_len - pos) + 3; // 3 bytes relacionados ao header
        }
      } 
      break;

      case PTS_is_BOTH: /* Tem informacoes PTS/DTS (Presentation/Decode Time Stamp) */
      {
        ATV_DEBUG("The PTS Extension is (PTS_is_BOTH)");

        // TODO: Caso precise implementar, favor ler no livro "Data Broadcast
        // pag 98, Tabela 3.12
      }
      break;

      case PTS_is_NONE: /* Nenhum */
    	  ATV_DEBUG("WORKAROUND for RedeTV! - handling PTS_is_NONE _ALMOST_ the same way as PTS_is_ONLY_PTS\n");
       	  uint32_t pts = ((pack[9] & 0x0E) << 29) | (pack[10] << 22) | ((pack[11] >> 1) << 15) | (pack[12] << 7) | (pack[13] >> 1);
       	  //uint8_t pos = 0x20; // posicao do 'payload'
       	  uint8_t pos = 0x1B;

       	  packet_pes_set_pts (&pts, &pes->pts);

       	  pes->payload.header = (pack[pos] << 16) | (pack[pos+1] << 8) | pack[pos+2];
       	  pes->payload.data   = &pack[pos + 3];
       	  pes->payload.len    = (pes->pack_len - pos) + 3; // 3 bytes relacionados ao header
       	  break;
      default:
        ATV_DEBUG ("Invalid PTS? PTS_DTS_flag = %d (%#x) PES is NONE, ...",
          pes->header.flags.bit.PTS_DTS_flag, pes->header.flags.bit.PTS_DTS_flag);
        return FALSE;
    }

    // TODO: Caso necessario, basta implementar os esquemas para ESCR_flag
    /*
    switch (pes->header.flags.bit.ESCR_flag) {
    ...
    }*/
  }

#ifdef DEBUG_PACKET_PES // apenas para ajudar na depuracao
  ATV_HEXDUMP (pack, pes->pack_len+6);
  ATV_DEBUG("\n"
            "<PacketPES_t>\n"
            "  stream_id = %#x\n"
            "  pack_len  = %#x\n"
            "  struct header {\n"
            "    len = %d (%#x)\n"
            "    raw = %#x\n"
            "    struct flags {\n"
            "      rights              = %#x\n"
            "      copyright           = %#x\n"
            "      aligment            = %#x\n"
            "      priority            = %#x\n"
            "      scrambling          = %#x\n"
            "      emulation           = %#x\n"
            "      PES_extension_flag  = %#x\n"
            "      PES_CRC_flag        = %#x\n"
            "      copy_info_flag      = %#x\n"
            "      DSM_trick_mode_flag = %#x\n"
            "      ES_rate_flag        = %#x\n"
            "      ESCR_flag           = %#x\n"
            "      PTS_DTS_flag        = %#x\n"
            "    };\n"
            "  };\n"
            "  struct payload {\n"
            "    header = %#x\n"
            "    data   = %#x\n"
            "    len    = %d (%#x)\n"
            "  };\n"
            "</PacketPES_t>",
            pes->stream_id, pes->pack_len, pes->header.len, pes->header.len, pes->header.flags.raw,
            pes->header.flags.bit.rights, pes->header.flags.bit.copyright, pes->header.flags.bit.alignment, 
            pes->header.flags.bit.priority, pes->header.flags.bit.scrambling, pes->header.flags.bit.emulation,
            pes->header.flags.bit.PES_extension_flag, pes->header.flags.bit.PES_CRC_flag, pes->header.flags.bit.copy_info_flag, 
            pes->header.flags.bit.DSM_trick_mode_flag, pes->header.flags.bit.ES_rate_flag, pes->header.flags.bit.ESCR_flag, 
            pes->header.flags.bit.PTS_DTS_flag, pes->payload.header, pes->payload.data, pes->payload.len, pes->payload.len
  );
#endif

  return TRUE;
}

void
packet_pes_set_pts (uint32_t *ts,
                    PES_pts_t *pts)
{
  uint64_t p = 0;
  
  assert (pts != NULL);
  assert (ts != NULL);

  pts->ts = *ts;
  p       = pts->ts / 9;
  pts->h  = (p/(10000L*60*60));
  pts->m  = (p/(10000L*60))-(pts->h*60);
  pts->s  = (p/10000L)-(pts->h*3600)-(pts->m*60);
  pts->u  = (p-(pts->h*10000L*60*60)-(pts->m*10000L*60)-(pts->s*10000L));
}

