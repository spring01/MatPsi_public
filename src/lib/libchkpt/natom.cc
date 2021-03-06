/*
 *@BEGIN LICENSE
 *
 * PSI4: an ab initio quantum chemistry software package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *@END LICENSE
 */

/*!
  \file
  \ingroup CHKPT
*/

#include <cstdlib>
#include <psifiles.h>
#include <boost/shared_ptr.hpp>
#include <libpsio/psio.hpp>
#include <libchkpt/chkpt.h>
#include <libchkpt/chkpt.hpp>

using namespace psi;

int Chkpt::rd_natom(void)
{
        int natom;
        char *keyword;
        keyword = build_keyword("Num. atoms");

        psio->read_entry(PSIF_CHKPT, keyword, (char *) &natom, sizeof(int));

        free(keyword);
        return natom;
}

void Chkpt::wt_natom(int natom)
{
        char *keyword;
        keyword = build_keyword("Num. atoms");

        psio->write_entry(PSIF_CHKPT, keyword, (char *) &natom, sizeof(int));

        free(keyword);
}

extern "C" {
/*!
** int chkpt_rd_natom()
** Reads in the total number of atoms.
**
** Parameters: none
**
** Returns:
**   natom = total number of atoms.
** \ingroup CHKPT
*/
        int chkpt_rd_natom(void)
        {
                return _default_chkpt_lib_->rd_natom();
        }

/*!
** void chkpt_wt_natom(int)
** Writes out the total number of atoms.
**
** Parameters:
**   \param natom = total number of atoms.
**
** Returns: none
** \ingroup CHKPT
*/
        void chkpt_wt_natom(int natom)
        {
                _default_chkpt_lib_->wt_natom(natom);
        }
}

