/*
 * _compilerWorkAround.h
 *
 * URL:      https://github.com/panchaBhuta/converter
 * Version:  v1.0
 *
 * Copyright (c) 2023-2026 Gautam Dhar
 * All rights reserved.
 *
 * converter is distributed under the BSD 3-Clause license, see LICENSE for details.
 *
 */

#pragma once


/*
 * Compiler-specific workarounds
 *
 * GCC 12.3 has an internal compiler error (Segmentation fault) when
 * matching the constrained template-template parameter used by
 * isBumpedTypeConversionAvailable.
 *
 * GCC 12.4 and later handle the same construct correctly.
 *
 * Keep the stronger c_numeric constraint for all unaffected compilers /
 * compiler versions. On the affected GCC versions, use 'typename' only
 * for the template-template parameter so that the capability trait can
 * still be instantiated without triggering the compiler bug.
 *
 * This macro is intentionally defined here as a preprocessor token.
 * It does not require the c_numeric concept to be declared at this point;
 * the C++ parser sees either 'typename' or 'c_numeric' after preprocessing.
 */
#if defined(__GNUC__) && !defined(__clang__)     && ((__GNUC__ < 12) ||         (__GNUC__ == 12 && __GNUC_MINOR__ < 4))

#define CONVERTER_TEMPLATE_TYPE_CONSTRAINT typename

#else

#define CONVERTER_TEMPLATE_TYPE_CONSTRAINT c_numeric

#endif
