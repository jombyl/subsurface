#ifndef UNITS_H
#define UNITS_H

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#define O2_IN_AIR 209 // permille
#define N2_IN_AIR 781
#define O2_DENSITY 1429 // mg/Liter
#define N2_DENSITY 1251
#define HE_DENSITY 179
#define SURFACE_PRESSURE 1013 // mbar
#define SURFACE_PRESSURE_STRING "1013"
#define ZERO_C_IN_MKELVIN 273150 // mKelvin

/* Salinity is expressed in weight in grams per 10l */
#define SEAWATER_SALINITY 10300
#define FRESHWATER_SALINITY 10000

/*
 * Some silly typedefs to make our units very explicit.
 *
 * Also, the units are chosen so that values can be expressible as
 * integers, so that we never have FP rounding issues. And they
 * are small enough that converting to/from imperial units doesn't
 * really matter.
 *
 * We also strive to make '0' a meaningless number saying "not
 * initialized", since many values are things that may not have
 * been reported (eg cylinder pressure or temperature from dive
 * computers that don't support them). But sometimes -1 is an even
 * more explicit way of saying "not there".
 *
 * Thus "millibar" for pressure, for example, or "millikelvin" for
 * temperatures. Doing temperatures in celsius or fahrenheit would
 * make for loss of precision when converting from one to the other,
 * and using millikelvin is SI-like but also means that a temperature
 * of '0' is clearly just a missing temperature or cylinder pressure.
 *
 * Also strive to use units that can not possibly be mistaken for a
 * valid value in a "normal" system without conversion. If the max
 * depth of a dive is '20000', you probably didn't convert from mm on
 * output, or if the max depth gets reported as "0.2ft" it was either
 * a really boring dive, or there was some missing input conversion,
 * and a 60-ft dive got recorded as 60mm.
 *
 * Doing these as "structs containing value" means that we always
 * have to explicitly write out those units in order to get at the
 * actual value. So there is hopefully little fear of using a value
 * in millikelvin as Fahrenheit by mistake.
 *
 * We don't actually use these all yet, so maybe they'll change, but
 * I made a number of types as guidelines.
 */
typedef int64_t timestamp_t;

typedef struct
{
	int seconds;
} duration_t;

typedef struct
{
	int mm;
} depth_t;

typedef struct
{
	int mbar;
} pressure_t;

typedef struct
{
	int mkelvin;
} temperature_t;

typedef struct
{
	int mliter;
} volume_t;

typedef struct
{
	int permille;
} fraction_t;

typedef struct
{
	int grams;
} weight_t;

typedef struct
{
	int udeg;
} degrees_t;

static inline double grams_to_lbs(int grams)
{
	return grams / 453.6;
}

static inline int lbs_to_grams(double lbs)
{
	return rint(lbs * 453.6);
}

static inline double ml_to_cuft(int ml)
{
	return ml / 28316.8466;
}

static inline double cuft_to_l(double cuft)
{
	return cuft * 28.3168466;
}

static inline double mm_to_feet(int mm)
{
	return mm * 0.00328084;
}

static inline unsigned long feet_to_mm(double feet)
{
	return rint(feet * 304.8);
}

static inline int to_feet(depth_t depth)
{
	return rint(mm_to_feet(depth.mm));
}

static inline double mkelvin_to_C(int mkelvin)
{
	return (mkelvin - ZERO_C_IN_MKELVIN) / 1000.0;
}

static inline double mkelvin_to_F(int mkelvin)
{
	return mkelvin * 9 / 5000.0 - 459.670;
}

static inline unsigned long F_to_mkelvin(double f)
{
	return rint((f - 32) * 1000 / 1.8 + ZERO_C_IN_MKELVIN);
}

static inline unsigned long C_to_mkelvin(double c)
{
	return rint(c * 1000 + ZERO_C_IN_MKELVIN);
}

static inline double psi_to_bar(double psi)
{
	return psi / 14.5037738;
}

static inline long psi_to_mbar(double psi)
{
	return rint(psi_to_bar(psi) * 1000);
}

static inline int to_PSI(pressure_t pressure)
{
	return rint(pressure.mbar * 0.0145037738);
}

static inline double bar_to_atm(double bar)
{
	return bar / SURFACE_PRESSURE * 1000;
}

static inline double mbar_to_atm(int mbar)
{
	return (double)mbar / SURFACE_PRESSURE;
}

static inline int mbar_to_PSI(int mbar)
{
	pressure_t p = { mbar };
	return to_PSI(p);
}

/*
 * We keep our internal data in well-specified units, but
 * the input and output may come in some random format. This
 * keeps track of those units.
 */
/* turns out in Win32 PASCAL is defined as a calling convention */
#ifdef WIN32
#undef PASCAL
#endif
struct units {
	enum {
		METERS,
		FEET
	} length;
	enum {
		LITER,
		CUFT
	} volume;
	enum {
		BAR,
		PSI,
		PASCAL
	} pressure;
	enum {
		CELSIUS,
		FAHRENHEIT,
		KELVIN
	} temperature;
	enum {
		KG,
		LBS
	} weight;
	enum {
		SECONDS,
		MINUTES
	} vertical_speed_time;
};

/*
 * We're going to default to SI units for input. Yes,
 * technically the SI unit for pressure is Pascal, but
 * we default to bar (10^5 pascal), which people
 * actually use. Similarly, C instead of Kelvin.
 * And kg instead of g.
 */
#define SI_UNITS                                                                                                                         \
	{                                                                                                                                \
		.length = METERS, .volume = LITER, .pressure = BAR, .temperature = CELSIUS, .weight = KG, .vertical_speed_time = MINUTES \
	}

#define IMPERIAL_UNITS                                                                                                                    \
	{                                                                                                                                 \
		.length = FEET, .volume = CUFT, .pressure = PSI, .temperature = FAHRENHEIT, .weight = LBS, .vertical_speed_time = MINUTES \
	}

#ifdef __cplusplus
}
#endif

#endif