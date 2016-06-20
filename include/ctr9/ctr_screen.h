#ifndef CTR_SCREEN_H_
#define CTR_SCREEN_H_

typedef enum
{
	CTR_SCREEN_NONE = 0,
	CTR_SCREEN_TOP = 1,
	CTR_SCREEN_BOTTOM = 2,
	CTR_SCREEN_BOTH = 3
} ctr_screen_enum;

/**	@brief Enables the backlight for the screens specified.
 *
 *	@param[in] aScreens Screen backlights to enable.
 *
 *	@post Backlight for the specified screens will be enabled.
 */
void ctr_screen_enable_backlight(ctr_screen_enum aScreens);

/**	@brief Disables the backlight for the screens specified.
 *
 *	@param[in] aScreens Screen backlights to disable.
 *
 *	@post Backlight for the specified screens will be disabled.
 */
void ctr_screen_disable_backlight(ctr_screen_enum aScreens);


#endif//CTR_SCREEN_H_

