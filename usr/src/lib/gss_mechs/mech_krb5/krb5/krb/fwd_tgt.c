/*
 * Copyright (c) 1999, 2010, Oracle and/or its affiliates. All rights reserved.
 */
/*
 * lib/krb5/krb/get_in_tkt.c
 *
 * Copyright 1995 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 *
 * Export of this software from the United States of America may
 *   require a specific license from the United States Government.
 *   It is the responsibility of any person or organization contemplating
 *   export to obtain such a license before exporting.
 *
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that
 * the name of M.I.T. not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  Furthermore if you modify this software you must label
 * your software as modified software and not distribute it in such a
 * fashion that it might be confused with the original M.I.T. software.
 * M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 */

#include "k5-int.h"
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#include <locale.h>

/* helper function: convert flags to necessary KDC options */
#define flags2options(flags) (flags & KDC_TKT_COMMON_MASK)

/* Get a TGT for use at the remote host */
krb5_error_code KRB5_CALLCONV
krb5_fwd_tgt_creds(krb5_context context, krb5_auth_context auth_context, char *rhost, krb5_principal client, krb5_principal server, krb5_ccache cc, int forwardable, krb5_data *outbuf)






                          /* Should forwarded TGT also be forwardable? */

{
    krb5_replay_data replaydata;
    krb5_data * scratch = 0;
    krb5_address **addrs = NULL;
    krb5_error_code retval;
    krb5_creds creds, tgt;
    krb5_creds *pcreds;
    krb5_flags kdcoptions;
    int close_cc = 0;
    int free_rhost = 0;
    krb5_enctype enctype = 0;
    krb5_keyblock *session_key;
    krb5_boolean old_use_conf_ktypes = context->use_conf_ktypes;

    memset((char *)&creds, 0, sizeof(creds));
    memset((char *)&tgt, 0, sizeof(creds));

    if (cc == 0) {
      if ((retval = krb5int_cc_default(context, &cc)))
	goto errout;
      close_cc = 1;
    }
    retval = krb5_auth_con_getkey (context, auth_context, &session_key);
    if (retval)
      goto errout;
    if (session_key) {
	enctype = session_key->enctype;
	krb5_free_keyblock (context, session_key);
	session_key = NULL;
    } else if (server) { /* must server be non-NULL when rhost is given? */
	/* Try getting credentials to see what the remote side supports.
	   Not bulletproof, just a heuristic.  */
	krb5_creds in, *out = 0;
	memset (&in, 0, sizeof(in));

	retval = krb5_copy_principal (context, server, &in.server);
	if (retval)
	    goto punt;
	retval = krb5_copy_principal (context, client, &in.client);
	if (retval)
	    goto punt;
	retval = krb5_get_credentials (context, 0, cc, &in, &out);
	if (retval)
	    goto punt;
	/* Got the credentials.  Okay, now record the enctype and
	   throw them away.  */
	enctype = out->keyblock.enctype;
	krb5_free_creds (context, out);
    punt:
	krb5_free_cred_contents (context, &in);
    }

    if ((retval = krb5_copy_principal(context, client, &creds.client)))
	goto errout;

    if ((retval = krb5_build_principal_ext(context, &creds.server,
					   client->realm.length,
					   client->realm.data,
					   KRB5_TGS_NAME_SIZE,
					   KRB5_TGS_NAME,
					   client->realm.length,
					   client->realm.data,
					   0)))
	goto errout;

    /* fetch tgt directly from cache */
    context->use_conf_ktypes = 1;
    retval = krb5_cc_retrieve_cred (context, cc, KRB5_TC_SUPPORTED_KTYPES,
				    &creds, &tgt);
    context->use_conf_ktypes = old_use_conf_ktypes;
    if (retval)
	goto errout;

    /* tgt->client must be equal to creds.client */
    if (!krb5_principal_compare(context, tgt.client, creds.client)) {
        /* Solaris Kerberos */
        char *r_name = NULL;
	char *t_name = NULL;
	krb5_error_code r_err, t_err;
	t_err = krb5_unparse_name(context, tgt.client, &t_name);
	r_err = krb5_unparse_name(context, creds.client, &r_name);
	krb5_set_error_message(context, KRB5_PRINC_NOMATCH,
			    dgettext(TEXT_DOMAIN,
				    "Requested principal and ticket don't match:  Requested principal is '%s' and TGT principal is '%s'"),
			    r_err ? "unknown" : r_name,
			    t_err ? "unknown" : t_name);
	if (r_name)
	    krb5_free_unparsed_name(context, r_name);
	if (t_name)
	    krb5_free_unparsed_name(context, t_name);
	retval = KRB5_PRINC_NOMATCH;
	goto errout;
    }

    if (!tgt.ticket.length) {
	retval = KRB5_NO_TKT_SUPPLIED;
	goto errout;
    }

    if (tgt.addresses && *tgt.addresses) {
      if (rhost == NULL) {
	if (krb5_princ_type(context, server) != KRB5_NT_SRV_HST) {
retval = KRB5_FWD_BAD_PRINCIPAL;
 goto errout;
	}

	if (krb5_princ_size(context, server) < 2){
	  retval = KRB5_CC_BADNAME;
	  goto errout;
	}

	rhost = malloc(server->data[1].length+1);
	if (!rhost) {
	  retval = ENOMEM;
	  goto errout;
	}
	free_rhost = 1;
	/* Solaris Kerberos */
	(void) memcpy(rhost, server->data[1].data, server->data[1].length);
	rhost[server->data[1].length] = '\0';
      }

	retval = krb5_os_hostaddr(context, rhost, &addrs);
	if (retval)
	    goto errout;
    }

    creds.keyblock.enctype = enctype;
    creds.times = tgt.times;
    creds.times.starttime = 0;
    kdcoptions = flags2options(tgt.ticket_flags)|KDC_OPT_FORWARDED;

    if (!forwardable) /* Reset KDC_OPT_FORWARDABLE */
      kdcoptions &= ~(KDC_OPT_FORWARDABLE);

    if ((retval = krb5_get_cred_via_tkt(context, &tgt, kdcoptions,
					addrs, &creds, &pcreds))) {
	if (enctype) {
	    creds.keyblock.enctype = 0;
	    if ((retval = krb5_get_cred_via_tkt(context, &tgt, kdcoptions,
						addrs, &creds, &pcreds)))
		goto errout;
	}
	else goto errout;
    }
    retval = krb5_mk_1cred(context, auth_context, pcreds,
                           &scratch, &replaydata);
    krb5_free_creds(context, pcreds);

    /*
     * Solaris Kerberos: changed this logic from the MIT 1.2.1 version to be
     * more robust.
     */
    if (scratch) {
	if (retval)
	    krb5_free_data(context, scratch);
	else {
	    *outbuf = *scratch;
	    krb5_xfree(scratch);
	}
    }

errout:
    if (addrs)
	krb5_free_addresses(context, addrs);
    /* Solaris Kerberos */
    if (close_cc)
	(void) krb5_cc_close(context, cc);
    if (free_rhost)
	free(rhost);
    krb5_free_cred_contents(context, &creds);
    krb5_free_cred_contents(context, &tgt);
    return retval;
}
