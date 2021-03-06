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
 \ingroup PSIO
 */

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <libpsio/psio.h>
#include <libpsio/psio.hpp>
#include <boost/shared_ptr.hpp>
#include <psi4-dec.h>
#include <psifiles.h>

#include <unistd.h>

#include <boost/filesystem.hpp>

namespace psi {

/* Definition of global data */
boost::shared_ptr<PSIO> _default_psio_lib_;
boost::shared_ptr<PSIOManager> _default_psio_manager_;
std::string PSIO::default_namespace_;

int PSIO::_error_exit_code_ = 1;
psio_address PSIO_ZERO = { 0, 0 };

PSIO::PSIO()
{
    _psio_manager_ = boost::shared_ptr<PSIOManager>(new PSIOManager);
    int i, j;

    psio_unit = (psio_ud *) malloc(sizeof(psio_ud)*PSIO_MAXUNIT);
#ifdef PSIO_STATS
    psio_readlen = (ULI *) malloc(sizeof(ULI) * PSIO_MAXUNIT);
    psio_writlen = (ULI *) malloc(sizeof(ULI) * PSIO_MAXUNIT);
#endif
    state_ = 1;

    if (psio_unit == NULL) {
        fprintf(stderr, "Error in PSIO_INIT()!\n");
        exit(_error_exit_code_);
    }

    for (i=0; i < PSIO_MAXUNIT; i++) {
#ifdef PSIO_STATS
        psio_readlen[i] = psio_writlen[i] = 0;
#endif
        psio_unit[i].numvols = 0;
        for (j=0; j < PSIO_MAXVOL; j++) {
            psio_unit[i].vol[j].path = NULL;
            psio_unit[i].vol[j].stream = -1;
        }
        psio_unit[i].toclen = 0;
        psio_unit[i].toc = NULL;
    }

    /* Open user's general .psirc file, if exists */
    //  char *userhome = getenv("HOME");
    //  char *filename = (char*) malloc((strlen(userhome)+8)*sizeof(char));
    //  sprintf(filename, "%s%s", userhome, "/.psirc");
    //  FILE *psirc = fopen(filename, "r");
    //  if (psirc != NULL) {
    //    ip_append(psirc, stdout);
    //    fclose(psirc);
    //  }
    //  free(filename);

    /*
   implement some default PSI3 behavior:
   1) checkpoint file should by default be in "./"
   2) all other files should go to "/tmp/"
   3) default name is psi_file_prefix
   4) 1 volume
   */
    boost::filesystem::path tempdir = boost::filesystem::temp_directory_path();
    std::string tempdir_str = tempdir.native();
    tempdir_str += "/";
    for (i=1; i<=PSIO_MAXVOL; ++i) {
        char kwd[20];
        sprintf(kwd, "VOLUME%u", i);
        filecfg_kwd("DEFAULT", kwd, PSIF_CHKPT, "./");
        filecfg_kwd("DEFAULT", kwd, -1, tempdir_str.c_str());
    }
    filecfg_kwd("DEFAULT", "NAME", -1, psi_file_prefix);
    filecfg_kwd("DEFAULT", "NVOLUME", -1, "1");


    // Get the process ID and convert to string.
    pid_t pid = getpid();
    std::stringstream ss;
    ss << pid;
    pid_ = ss.str();
}

boost::shared_ptr<PSIO> PSIO::shared_object()
{
    return _default_psio_lib_;
}

int psio_init(void) {
    if (_default_psio_lib_.get() == 0) {
        boost::shared_ptr<PSIO> temp(new PSIO);
        _default_psio_lib_ = temp;
        if (_default_psio_lib_ == 0) {
            fprintf(stderr,"LIBPSIO::init() -- failed to allocate the memory");
            exit(PSIO::_error_exit_code_);
        }
    }
    if (_default_psio_manager_.get() == 0) {
        boost::shared_ptr<PSIOManager> temp(new PSIOManager);
        _default_psio_manager_ = temp;
        if (_default_psio_manager_ == 0) {
            fprintf(stderr,"LIBPSIO::init() -- failed to allocate the memory");
            exit(PSIO::_error_exit_code_);
        }
    }

    return 1;
}

int psio_state() {
    return _default_psio_lib_->state();
}

}

