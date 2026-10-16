#ifndef CAPTION_VERSION_H_
#define CAPTION_VERSION_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file caption-version.h
 * @brief Library version (semver, same as the VERSION file).
 */

/**
 * @name Compile time version
 * Only for those who want to check it with @c \#if.
 * @{
 */
#define CLOSEDCAPTION_VERSION_MAJOR 1
#define CLOSEDCAPTION_VERSION_MINOR 0
#define CLOSEDCAPTION_VERSION_PATCH 1
/** @} */

/**
 * Version of the lib that is loaded at runtime, e.g.: "1.0.1".
 *
 * Good to compare with the CLOSEDCAPTION_VERSION_* of the header used in the build.
 *
 * @return Static string with the version (no need to free it).
 */
const char *closedcaption_version (void);

#ifdef __cplusplus
}
#endif

#endif /* CAPTION_VERSION_H_ */
