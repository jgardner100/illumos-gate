/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */

/*
 * Copyright 2023 Oxide Computer Company
 */

syscall:::entry
{
	/*
	 * The result of this strstr() should of course always be non-NULL --
	 * but prior to the fix for illumos#15789, it... wasn't (with the
	 * variable nomenclature here speaking for itself).
	 */
	this->wtaf = (strstr("seriously wtf", "wtf") == NULL);
}

syscall:::entry
/this->wtaf/
{
	printf("but seriously though");
	exit(1);
}

tick-1sec
/n++ > 10/
{
	exit(0);
}
