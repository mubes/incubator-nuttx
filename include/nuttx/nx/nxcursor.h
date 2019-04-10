/****************************************************************************
 * include/nuttx/nx/cursor.h
 *
 *   Copyright (C) 2019 Gregory Nutt. All rights
 *     reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef __INCLUDE_NUTTX_NX_NXCURSOR_H
#define __INCLUDE_NUTTX_NX_NXCURSOR_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <stdbool.h>

#include <nuttx/nx/nx.h>
#include <nuttx/nx/nxtypes.h>

#ifndef defined(CONFIG_NX_SWCURSOR) || defined(CONFIG_NX_HWCURSOR)

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/* The current software cursor implementation is only available under the
 * following conditions:
 *
 * 1. Using a framebuffer hardware interface.  This is because the logic to
 *    implement this feature on top of the LCD interface has not been
 *    implemented.
 * 2. Pixel depth is greater then or equal to 8-bits (8-bpp, 16-bpp,
 *    24/32/-bpp).  This is because the logic to handle pixels smaller than
 *    1-byte has not been implemented,
 * 3. For FLAT and PROTECTED builds only.  In those builds, the cursor
 *    image resides in the common application space and is assumed to pesist
 *    as long as needed.  But with the KERNEL build, the image will lie in
 *    a process space and will not be generally available.  In that case,
 *    we could keep the image in a shared memory region or perhaps copy the
 *    image into a kernel internal buffer.  Neither of those are implemented.
 * 4. Only a single color plane is supported at present.
 */

#if (defined(CONFIG_NX_SWCURSOR) && \
    (defined(CONFIG_NX_LCDDRIVER) || !defined(CONFIG_NX_DISABLE_1BPP) || \
    !defined(CONFIG_NX_DISABLE_2BPP) || !defined(CONFIG_NX_DISABLE_4BPP) || \
     defined(CONFIG_BUILD_KERNEL) || CONFIG_NX_NPLANES > 1))
#  undef CONFIG_NX_NOCURSOR
#  undef CONFIG_NX_SWCURSOR
#  define CONFIG_NX_NOCURSOR 1
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/* For cursor controllers that support custem cursor images, this structure
 * is used to provide the cursor image.
 *
 * The image is provided a a 2-bits-per-pixel image.  The two bit incoding
 * is as followings:
 *
 * 00 - The transparent background
 * 01 - Color1:  The main color of the cursor
 * 10 - Color2:  The color of any border
 * 11 - Color3:  A blend color for better imaging (fake anti-aliasing).
 */

struct nx_cursorimage_s
{
  struct nxgl_size_s size;                  /* The size of the cursor image */
  nxgl_mxpixel_t color1[CONFIG_NX_NPLANES]; /* Color1 is main color of the cursor */
  nxgl_mxpixel_t color2[CONFIG_NX_NPLANES]; /* Color2 is color of any border */
  nxgl_mxpixel_t color3[CONFIG_NX_NPLANES]; /* Color3 is the blended color */
  FAR const uint8_t *image;                 /* Pointer to bitmap image data */
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: nxcursor_enable
 *
 * Description:
 *   Enable/disable presentation of the cursor
 *
 * Input Parameters:
 *   hnd    - The server handle returned by nx_connect()
 *   enable - True: show the cursor, false: hide the cursor.
 *
 * Returned Value:
 *   OK on success; ERROR on failure with errno set appropriately
 *
 ****************************************************************************/

int nxcursor_enable(NXHANDLE hnd, bool enable);

/****************************************************************************
 * Name: nxcursor_setimage
 *
 * Description:
 *   Set the cursor image.
 *
 *   The image is provided a a 2-bits-per-pixel image.  The two bit incoding
 *   is as followings:
 *
 *   00 - The transparent background
 *   01 - Color1:  The main color of the cursor
 *   10 - Color2:  The color of any border
 *   11 - Color3:  A blend color for better imaging (fake anti-aliasing).
 *
 *   NOTE: The NX logic will reference the user image buffer repeatedly.
 *   That image buffer must persist for as long as the NX server connection
 *   persists.
 *
 * Input Parameters:
 *   hnd   - The server handle returned by nx_connect()
 *   image - Describes the cursor image in the expected format.
 *
 * Returned Value:
 *   OK on success; ERROR on failure with errno set appropriately
 *
 ****************************************************************************/

#if defined(CONFIG_NX_HWCURSORIMAGE) || defined(CONFIG_NX_SWCURSOR)
int nxcursor_setimage(NXHANDLE hnd, FAR struct nx_cursorimage_s *image);
#endif

/****************************************************************************
 * Name: nxcursor_setposition
 *
 * Description:
 *   Move the cursor to the specified position
 *
 * Input Parameters:
 *   hnd - The server handle returned by nx_connect()
 *   pos - The new cursor position
 *
 * Returned Value:
 *   OK on success; ERROR on failure with errno set appropriately
 *
 ****************************************************************************/

int nxcursor_setposition(NXHANDLE hnd, FAR const struct nxgl_point_s *pos);

/****************************************************************************
 * Name: nxcursor_get_position
 *
 * Description:
 *   Return the current cursor position.
 *
 *   CAUTION:  The current cursor position is not updated until the display
 *   is actually changed.  Due to asynchronies caused by queue, the new
 *   current cursor position may not match the cursor position set until
 *   the client and server are syncrhonized.
 *
 * Input Parameters:
 *   hnd - The server handle returned by nx_connect()
 *   pos - The location to return the cursor position
 *
 * Returned Value:
 *   OK on success; ERROR on failure with errno set appropriately
 *
 ****************************************************************************/

int nxcursor_get_position(NXHANDLE hnd, FAR struct nxgl_point_s *pos);

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* CONFIG_NX_SWCURSOR || CONFIG_NX_HWCURSOR */
#endif /* __INCLUDE_NUTTX_NX_NXCURSOR_H */

