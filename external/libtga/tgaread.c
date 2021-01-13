/*
 *  tgaread.c
 *
 *  Copying (C) 2001-2002  Matthias Brueckner <matbrc@gmx.de>
 *  This file is part of the struct TGA library (libtga).
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include "tga.h"


size_t
TGARead(TGA    *tga, 
	tbyte  *buf,
	size_t 	size, 
	size_t 	n)
{
	size_t read = fread(buf, size, n, tga->fd);
	tga->off = ftell(tga->fd);
	return read;
}

int 
TGAReadImage(TGA     *tga, 
	     TGAData *data)
{
	if (!tga) return 0;

	if (TGAReadHeader(tga) != TGA_OK) {
		TGA_ERROR(tga, tga->last);
		return 0;
	}

	if ((data->flags & TGA_IMAGE_ID) && tga->hdr.id_len != 0) {
		if (TGAReadImageId(tga, &data->img_id) != TGA_OK) {
			data->flags &= ~TGA_IMAGE_ID;
			TGA_ERROR(tga, tga->last);
		}
	} else {
		data->flags &= ~TGA_IMAGE_ID;
	}

	if (data->flags & TGA_IMAGE_DATA) {
		if (TGA_IS_MAPPED(tga)) {
			if (!TGAReadColorMap(tga, &data->cmap, data->flags)) {
				data->flags &= ~TGA_COLOR_MAP;
				TGA_ERROR(tga, tga->last);
				return 0;
			} else {
				data->flags |= TGA_COLOR_MAP;
			}
		}
		
		data->img_data = (tbyte*)malloc(TGA_IMG_DATA_SIZE(tga));
		if (!data->img_data) {
			data->flags &= ~TGA_IMAGE_DATA;
			TGA_ERROR(tga, TGA_OOM);
			return 0;
		}
		
		if (TGAReadScanlines(tga, data->img_data, 0, tga->hdr.height, data->flags) != tga->hdr.height) {
			data->flags &= ~TGA_IMAGE_DATA;
			TGA_ERROR(tga, tga->last);
		}
	}
	tga->last = TGA_OK;
	return TGA_OK;
}

int
TGAReadHeader (TGA *tga)
{
	tbyte *tmp;
	
	if (!tga) return 0;

	__TGASeek(tga, 0, SEEK_SET);
	if (tga->off != 0) {
		TGA_ERROR(tga, TGA_SEEK_FAIL);
		return 0;
	}

	tmp = (tbyte*)malloc(TGA_HEADER_SIZE);
	if (!tmp) {
		TGA_ERROR(tga, TGA_OOM);
		return 0;
	}
		
	memset(tmp, 0, TGA_HEADER_SIZE);
		
	if (!TGARead(tga, tmp, TGA_HEADER_SIZE, 1)) {
		free(tmp);
		TGA_ERROR(tga, TGA_READ_FAIL);
		return 0;
	}

	tga->hdr.id_len 	= tmp[0];
	tga->hdr.map_t 		= tmp[1];
	tga->hdr.img_t 		= tmp[2];
	tga->hdr.map_first 	= tmp[3] + tmp[4] * 256;
	tga->hdr.map_len 	= tmp[5] + tmp[6] * 256;
	tga->hdr.map_entry	= tmp[7];
	tga->hdr.x 		= tmp[8] + tmp[9] * 256;
	tga->hdr.y 		= tmp[10] + tmp[11] * 256;
	tga->hdr.width 		= tmp[12] + tmp[13] * 256;
	tga->hdr.height 	= tmp[14] + tmp[15] * 256;
	tga->hdr.depth 		= tmp[16];
	tga->hdr.alpha		= tmp[17] & 0x0f;
	tga->hdr.horz	        = (tmp[17] & 0x10) ? TGA_TOP : TGA_BOTTOM;
	tga->hdr.vert	        = (tmp[17] & 0x20) ? TGA_RIGHT : TGA_LEFT;

	if (tga->hdr.map_t && tga->hdr.depth != 8) {
		TGA_ERROR(tga, TGA_UNKNOWN_SUB_FORMAT);
		free(tga);
		free(tmp);
		return 0;
	} 

	if (tga->hdr.depth != 8 && 
	    tga->hdr.depth != 15 && 
	    tga->hdr.depth != 16 &&
	    tga->hdr.depth != 24 &&
	    tga->hdr.depth != 32) 
	{
		TGA_ERROR(tga, TGA_UNKNOWN_SUB_FORMAT);
		free(tga);
		free(tmp);
		return 0;
	}

	free(tmp);
	tga->last = TGA_OK;
	return TGA_OK;
}

int
TGAReadImageId(TGA    *tga, 
	       tbyte **buf)
{
	if (!tga || tga->hdr.id_len == 0) return 0;
       
	__TGASeek(tga, TGA_HEADER_SIZE, SEEK_SET);
	if (tga->off != TGA_HEADER_SIZE) {
		TGA_ERROR(tga, TGA_SEEK_FAIL);
		return 0;
	}
	*buf = (tbyte*)malloc(tga->hdr.id_len);
	if (!buf) {
		TGA_ERROR(tga, TGA_OOM);
		return 0;
	}

	if (!TGARead(tga, *buf, tga->hdr.id_len, 1)) {
		free(buf);
		TGA_ERROR(tga, TGA_READ_FAIL);
		return 0;
	}

	tga->last = TGA_OK;
	return TGA_OK;
}

int
TGAReadColorMap (TGA 	  *tga, 
		 tbyte   **buf,
		 tuint32   flags)
{
	tlong i, n, off, read, tmp;
 
	if (!tga) return 0;

	n = TGA_CMAP_SIZE(tga);
	if (n <= 0) return 0;
	
	off = TGA_CMAP_OFF(tga);
	if (tga->off != off) __TGASeek(tga, off, SEEK_SET);
	if (tga->off != off) {
		TGA_ERROR(tga, TGA_SEEK_FAIL);
		return 0;
	}
	
	*buf = (tbyte*)malloc(n);
	if (!buf) { 
		TGA_ERROR(tga, TGA_OOM);
		return 0;
	}

	if ((read = TGARead(tga, *buf, n, 1)) != 1) {
		TGA_ERROR(tga, TGA_READ_FAIL);
		return 0;
	}	
		
	if (TGA_CAN_SWAP(tga->hdr.map_entry) && (flags & TGA_RGB)) {
		__TGAbgr2rgb(*buf, TGA_CMAP_SIZE(tga), tga->hdr.map_entry / 8);
	}
	
	if (tga->hdr.map_entry == 15 || tga->hdr.map_entry == 16) {
		n = read + read / 2;
		*buf = (tbyte*)realloc(*buf, n);
		if (!(*buf)) {
			TGA_ERROR(tga, TGA_OOM);
			return 0;
		}

		for(i = read - 1; i >= 0; i -= 2) {
			tmp = *buf[i - 1] + *buf[i] * 255;
			*buf[n - 2] = (tmp >> 10) & 0x1F;
			*buf[n - 1] = (tmp >> 5) & 0x1F;
			*buf[n] = (tmp >> 5) & 0x1F;
			n -= 3;
		}
	}
	
	tga->last = TGA_OK;
	return read;
}

int
TGAReadRLE(TGA   *tga, 
	   tbyte *buf)
{
	int head;
	char sample[4];
	tbyte k, repeat = 0, direct = 0, bytes = tga->hdr.depth / 8;
	tshort x;
	tshort width = tga->hdr.width;
	FILE *fd = tga->fd;
	
	if (!tga || !buf) return TGA_ERROR;

	for (x = 0; x < width; ++x) {
		if (repeat == 0 && direct == 0) {
			head = getc(fd);
			if (head == EOF) return TGA_ERROR;
			if (head >= 128) {
				repeat = head - 127;
				if (fread(sample, bytes, 1, fd) < 1) 
					return TGA_ERROR;
			} else {
				direct = head + 1;
			}
		}
		if (repeat > 0) {
			for (k = 0; k < bytes; ++k) buf[k] = sample[k];
			--repeat;
		} else {
			if (fread(buf, bytes, 1, fd) < 1) return TGA_ERROR;
			--direct;
		}
		buf += bytes;
	}
		
	tga->last = TGA_OK;
	return TGA_OK;
}

size_t
TGAReadScanlines(TGA 	*tga, 
		 tbyte  *buf, 
		 size_t  sln,
		 size_t  n,
		 tuint32 flags)
{	
	tlong i, off;
	size_t sln_size, read, tmp;

	if (!tga || !buf) return 0;

	sln_size = TGA_SCANLINE_SIZE(tga);
	off = TGA_IMG_DATA_OFF(tga) + (sln * sln_size);
	
	if (tga->off != off) __TGASeek(tga, off, SEEK_SET);
	if (tga->off != off) {
		TGA_ERROR(tga, TGA_SEEK_FAIL);
		return 0;
	}

	if(TGA_IS_ENCODED(tga)) {
		for(read = 0; read <= n; ++read) {
			if(TGAReadRLE(tga, buf + ((sln + read) * sln_size)) !=
				TGA_OK) break;
		}
		tga->hdr.img_t -= 8;
	} else {
		read = TGARead(tga, buf, sln_size, n);
	}
	if(read != n) {
		TGA_ERROR(tga, TGA_READ_FAIL);
		return read;
	}
	
	if (TGA_CAN_SWAP(tga->hdr.depth) && (flags & TGA_RGB)) {
		__TGAbgr2rgb(buf + (sln_size * sln), sln_size * n, 
			   tga->hdr.depth / 8);
	}
	
	if (tga->hdr.depth == 15 || tga->hdr.depth == 16) {
		n = read + read / 2;	
		buf = (tbyte*)realloc(buf, n);
		if (!buf) {
			TGA_ERROR(tga, TGA_OOM);
			return 0;
		}
	
		for(i = read - 1; i >= 0; i -= 2) {
			tmp = buf[i - 1] + buf[i] * 255;
			buf[n - 2] = (tmp >> 10) & 0x1F;
			buf[n - 1] = (tmp >> 5) & 0x1F;
			buf[n] = (tmp >> 5) & 0x1F;
			n -= 3;
		}
	}
	
	tga->last = TGA_OK;
	return read;
}
