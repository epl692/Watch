/*
    modules/meds.c: meds module for openchronos-ng

    Copyright (C) 2012 Eric Lynema <epl692@sdf.org>
 
	            http://www.openchronos-ng.sourceforge.net

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <openchronos.h>

#include <drivers/display.h>
#include <drivers/rtca.h>
#include <drivers/buzzer.h>

int med_alert = 0;
int prn = 0;
int prn_effects = 0;
uint8_t prn_time = 0;
uint8_t prn_cooldown = CONFIG_MOD_MEDS_PRN_COOLDOWN;
uint8_t am_hour = CONFIG_MOD_MEDS_AM;
uint8_t pm_hour = CONFIG_MOD_MEDS_PM;
uint8_t am_minute = CONFIG_MOD_MEDS_AM_MIN;
uint8_t pm_minute = CONFIG_MOD_MEDS_PM_MIN;
uint8_t bed_hour = CONFIG_MOD_MEDS_BED;
uint8_t bed_minute = CONFIG_MOD_MEDS_BED_MIN;
static uint8_t tmp_hh, tmp_mm; //, prn_hh, prn_mm, prn_effect_hh, prn_effect_mm;

//Prototypes
void med_tick();
void alarmCheck(uint8_t hour, uint8_t minute);
void prn_set(uint8_t);
void prn_tick();
void med_chimeTick();
void med_chime();

static void num_press()
{
	//Do Something	
	display_clear(0, 2);
	display_chars(0, LCD_SEG_L2_4_0, "DONE", SEG_ON);

	//Clear Notice, Update Display
    display_symbol(0, LCD_ICON_ALARM, SEG_OFF);
	med_alert = 0;
}

static void prn_press()
{
	prn = 1;
	prn_effects = 1;
	display_clear(0, 2);
	display_chars(0, LCD_SEG_L2_4_0, "PRN", SEG_ON);
	display_symbol(0, LCD_ICON_HEART, SEG_ON);
    prn_set(prn_cooldown * 60); //prn_cooldown in hours, prn_set is in minutes. Convert!
}

static void prn_check()
{
  			_printf(0, LCD_SEG_L1_3_0, "%u", prn_time);
			//_printf(0, LCD_SEG_L2_3_2, "%02u", (prn_time % 60));
}

static void meds_activate()
{
	display_chars(0, LCD_SEG_L2_4_0, "MEDS", SEG_ON);
}

static void meds_deactivate()
{
	display_clear(0, 2);
}

void mod_meds_init(void)
{
	sys_messagebus_register(med_tick, SYS_MSG_RTC_MINUTE);	
	menu_add_entry("MEDS", &prn_press, &prn_check, &num_press, NULL, NULL, NULL,
						&meds_activate,
						&meds_deactivate);
}

void med_tick()
{
	tmp_hh = rtca_time.hour;
	tmp_mm = rtca_time.min;

    //Track the PRN usage.
    prn_tick();
	
    //Moring Meds
    #ifdef CONFIG_MOD_MEDS_AM_ACTIVE
        alarmCheck(am_hour, am_minute);
    #endif

    //Evening Meds
    #ifdef CONFIG_MOD_MEDS_PM_ACTIVE
        alarmCheck(pm_hour, pm_minute);
    #endif

    //Bedtime Meds
    #ifdef CONFIG_MOD_MEDS_BED_ACTIVE
        alarmCheck(bed_hour, bed_minute);
    #endif

        med_chimeTick();
}

void med_chimeTick()
{
    	if(med_alert == 1)
		{
			display_symbol(0, LCD_ICON_ALARM, SEG_ON);
			if((tmp_mm % 10) == 0)
			{
				med_chime();
			}
		} else {
			display_symbol(0, LCD_ICON_ALARM, SEG_OFF);
		}

}

void prn_set(uint8_t minutes)
{
    prn_time = minutes;
    prn = 1;
    display_symbol(0, LCD_ICON_HEART, SEG_ON);
}

void prn_tick()
{
    //Do this if we have a prn in our system.
    if(prn == 1)
    {
        prn_time = prn_time - 1;
    }
    
    if(prn_time <= 0)
    {
        prn = 0;
    }
    
    if(prn == 1)
    {
        	    display_symbol(0, LCD_ICON_HEART, SEG_ON);
    } else {
        	    display_symbol(0, LCD_ICON_HEART, SEG_OFF);
    }
}

void med_chime()
{
	note chime[4] = {0x1901, 0x1904, 0x1908, 0x000F};
	buzzer_play(chime);
}

void alarmCheck(uint8_t hour, uint8_t minute)
{
    	if(tmp_hh == hour && tmp_mm == minute)
		{
			med_alert = 1;
		}
}
