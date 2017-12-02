#include "GenChopper.h"

namespace GenChopper {

/*******************************************************************************************************************
Cycling '74 License for Max-Generated Code for Export
Copyright (c) 2016 Cycling '74
The code that Max generates automatically and that end users are capable of exporting and using, and any
  associated documentation files (the “Software”) is a work of authorship for which Cycling '74 is the author
  and owner for copyright purposes.  A license is hereby granted, free of charge, to any person obtaining a
  copy of the Software (“Licensee”) to use, copy, modify, merge, publish, and distribute copies of the Software,
  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The Software is licensed to Licensee only for non-commercial use. Users who wish to make commercial use of the
  Software must contact the copyright owner to determine if a license for commercial use is available, and the
  terms and conditions for same, which may include fees or royalties. For commercial use, please send inquiries
  to licensing (at) cycling74.com.  The determination of whether a use is commercial use or non-commercial use is based
  upon the use, not the user. The Software may be used by individuals, institutions, governments, corporations, or
  other business whether for-profit or non-profit so long as the use itself is not a commercialization of the
  materials or a use that generates or is intended to generate income, revenue, sales or profit.
The above copyright notice and this license shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
  THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*******************************************************************************************************************/

// global noise generator
Noise noise;
static const int GENLIB_LOOPCOUNT_BAIL = 100000;


// The State struct contains all the state and procedures for the gendsp kernel
typedef struct State {
	CommonState __commonstate;
	Change __m_change_29;
	DCBlock __m_dcblock_24;
	Data m_rms_data_19;
	Data m_segment_data_22;
	Data m_offset_data_20;
	Data m_length_data_21;
	int __exception;
	int vectorsize;
	t_sample m_capture_18;
	t_sample m_play_offset_5;
	t_sample m_max_length_17;
	t_sample m_energy_sum_2;
	t_sample m_total_length_1;
	t_sample samplerate;
	t_sample m_min_length_16;
	t_sample m_knob_23;
	t_sample m_play_rms_4;
	t_sample m_hold_15;
	t_sample m_pitchedmode_13;
	t_sample m_play_segment_8;
	t_sample m_play_index_7;
	t_sample m_play_len_6;
	t_sample m_playmode_14;
	t_sample m_crossing_count_9;
	t_sample m_prev_input_3;
	t_sample m_write_index_10;
	t_sample m_freq_12;
	t_sample m_write_segment_11;
	// re-initialize all member variables;
	inline void reset(t_param __sr, int __vs) {
		__exception = 0;
		vectorsize = __vs;
		samplerate = __sr;
		m_total_length_1 = ((int)0);
		m_energy_sum_2 = ((int)0);
		m_prev_input_3 = ((int)0);
		m_play_rms_4 = ((t_sample)0.1);
		m_play_offset_5 = ((int)0);
		m_play_len_6 = ((int)0);
		m_play_index_7 = ((int)0);
		m_play_segment_8 = ((int)0);
		m_crossing_count_9 = ((int)0);
		m_write_index_10 = ((int)0);
		m_write_segment_11 = ((int)1);
		m_freq_12 = ((int)220);
		m_pitchedmode_13 = ((int)0);
		m_playmode_14 = ((int)0);
		m_hold_15 = ((int)0);
		m_min_length_16 = ((int)16);
		m_max_length_17 = ((int)500);
		m_capture_18 = ((int)1);
		m_rms_data_19.reset("rms_data", ((int)64), ((int)1));
		m_offset_data_20.reset("offset_data", ((int)64), ((int)1));
		m_length_data_21.reset("length_data", ((int)64), ((int)1));
		m_segment_data_22.reset("segment_data", ((int)10004), ((int)64));
		m_knob_23 = 0;
		__m_dcblock_24.reset();
		__m_change_29.reset(0);
		genlib_reset_complete(this);
		
	};
	// the signal processing routine;
	inline int perform(t_sample ** __ins, t_sample ** __outs, int __n) {
		vectorsize = __n;
		const t_sample * __in1 = __ins[0];
		t_sample * __out1 = __outs[0];
		if (__exception) {
			return __exception;
			
		} else if (( (__in1 == 0) || (__out1 == 0) )) {
			__exception = GENLIB_ERR_NULL_BUFFER;
			return __exception;
			
		};
		t_sample sub_1339 = (m_knob_23 - ((int)0));
		t_sample scale_1336 = ((safepow((sub_1339 * ((t_sample)1)), ((int)1)) * ((int)19)) + ((int)1));
		t_sample scale_429 = scale_1336;
		t_sample sub_1343 = (m_knob_23 - ((int)0));
		t_sample scale_1340 = ((safepow((sub_1343 * ((t_sample)1)), ((int)1)) * ((int)10)) + ((int)1));
		t_sample sub_1347 = (m_knob_23 - ((int)0));
		t_sample scale_1344 = ((safepow((sub_1347 * ((t_sample)1)), ((int)1)) * ((int)49)) + ((int)1));
		t_sample scale_428 = scale_1344;
		int segment_data_dim = m_segment_data_22.dim;
		int segment_data_channels = m_segment_data_22.channels;
		int num_segments = segment_data_channels;
		t_sample crossings = scale_429;
		t_sample repeats = scale_1340;
		t_sample rate = scale_428;
		// the main sample loop;
		while ((__n--)) {
			const t_sample in1 = (*(__in1++));
			t_sample unbiased_input = __m_dcblock_24(in1);
			m_energy_sum_2 = (m_energy_sum_2 + (unbiased_input * unbiased_input));
			m_write_index_10 = (m_write_index_10 + ((int)1));
			int channel_trunc_25 = fixnan(floor(m_write_segment_11));
			bool chan_ignore_26 = ((channel_trunc_25 < 0) || (channel_trunc_25 >= segment_data_channels));
			int index_trunc_27 = fixnan(floor(m_write_index_10));
			bool index_ignore_28 = ((index_trunc_27 >= segment_data_dim) || (index_trunc_27 < 0));
			if ((!(chan_ignore_26 || index_ignore_28))) {
				m_segment_data_22.write(unbiased_input, index_trunc_27, channel_trunc_25);
				
			};
			int is_crossing = (__m_change_29((unbiased_input > ((int)0))) > ((int)0));
			if (is_crossing) {
				if ((m_write_index_10 > m_max_length_17)) {
					m_crossing_count_9 = ((int)0);
					m_write_index_10 = ((int)0);
					
				} else {
					m_crossing_count_9 = (m_crossing_count_9 + ((int)1));
					int is_complete = ((m_capture_18 && (m_crossing_count_9 >= crossings)) && (m_write_index_10 >= m_min_length_16));
					if (is_complete) {
						t_sample offset = safediv(m_prev_input_3, (m_prev_input_3 - unbiased_input));
						int offset_data_dim = m_offset_data_20.dim;
						int offset_data_channels = m_offset_data_20.channels;
						int index_trunc_30 = fixnan(floor(m_write_segment_11));
						bool index_ignore_31 = ((index_trunc_30 >= offset_data_dim) || (index_trunc_30 < 0));
						// samples offset_data channel 1;
						t_sample prev_offset = (index_ignore_31 ? 0 : m_offset_data_20.read(index_trunc_30, 0));
						t_sample len = (((m_write_index_10 + offset) - prev_offset) - ((int)1));
						int length_data_dim = m_length_data_21.dim;
						int length_data_channels = m_length_data_21.channels;
						int index_trunc_32 = fixnan(floor(m_write_segment_11));
						bool index_ignore_33 = ((index_trunc_32 >= length_data_dim) || (index_trunc_32 < 0));
						// samples length_data channel 1;
						t_sample prev_length = (index_ignore_33 ? 0 : m_length_data_21.read(index_trunc_32, 0));
						m_total_length_1 = ((m_total_length_1 - prev_length) + len);
						int index_trunc_34 = fixnan(floor(m_write_segment_11));
						bool index_ignore_35 = ((index_trunc_34 >= length_data_dim) || (index_trunc_34 < 0));
						if ((!index_ignore_35)) {
							m_length_data_21.write(len, index_trunc_34, 0);
							
						};
						t_sample rms = sqrt(safediv(m_energy_sum_2, floor(len)));
						int rms_data_dim = m_rms_data_19.dim;
						int rms_data_channels = m_rms_data_19.channels;
						int index_trunc_36 = fixnan(floor(m_write_segment_11));
						bool index_ignore_37 = ((index_trunc_36 >= rms_data_dim) || (index_trunc_36 < 0));
						if ((!index_ignore_37)) {
							m_rms_data_19.write(rms, index_trunc_36, 0);
							
						};
						m_crossing_count_9 = ((int)0);
						m_energy_sum_2 = ((int)0);
						m_write_segment_11 = wrap((m_write_segment_11 + ((int)1)), ((int)0), num_segments);
						if ((m_write_segment_11 == m_play_segment_8)) {
							m_write_segment_11 = wrap((m_write_segment_11 + ((int)1)), ((int)0), num_segments);
							
						};
						int index_trunc_38 = fixnan(floor(m_write_segment_11));
						bool index_ignore_39 = ((index_trunc_38 >= offset_data_dim) || (index_trunc_38 < 0));
						if ((!index_ignore_39)) {
							m_offset_data_20.write(offset, index_trunc_38, 0);
							
						};
						int channel_trunc_40 = fixnan(floor(m_write_segment_11));
						bool chan_ignore_41 = ((channel_trunc_40 < 0) || (channel_trunc_40 >= segment_data_channels));
						if ((!chan_ignore_41)) {
							m_segment_data_22.write(m_prev_input_3, 0, channel_trunc_40);
							
						};
						int channel_trunc_42 = fixnan(floor(m_write_segment_11));
						bool chan_ignore_43 = ((channel_trunc_42 < 0) || (channel_trunc_42 >= segment_data_channels));
						bool index_ignore_44 = (((int)1) >= segment_data_dim);
						if ((!(chan_ignore_43 || index_ignore_44))) {
							m_segment_data_22.write(unbiased_input, ((int)1), channel_trunc_42);
							
						};
						m_write_index_10 = ((int)1);
						
					};
					
				};
				
			};
			m_prev_input_3 = unbiased_input;
			t_sample r = rate;
			if ((m_pitchedmode_13 < ((int)1))) {
				
			} else {
				if ((m_pitchedmode_13 < ((int)2))) {
					t_sample d = safediv(m_play_index_7, m_play_len_6);
					r = (rate * ((((int)1) < d) ? d : ((int)1)));
					
				} else {
					if ((m_pitchedmode_13 < ((int)3))) {
						t_sample d = ceil(safediv(m_play_index_7, m_play_len_6));
						t_sample maxb_45 = (d * d);
						r = safediv(rate, ((((int)1) < maxb_45) ? maxb_45 : ((int)1)));
						
					} else {
						r = safediv((m_freq_12 * m_play_len_6), (samplerate * crossings));
						
					};
					
				};
				
			};
			m_play_index_7 = (m_play_index_7 + r);
			t_sample actual_play_index = wrap(m_play_index_7, ((int)0), m_play_len_6);
			int index_trunc_46 = fixnan(floor((m_play_offset_5 + actual_play_index)));
			double index_fract_47 = ((m_play_offset_5 + actual_play_index) - index_trunc_46);
			int index_trunc_48 = (index_trunc_46 + 1);
			bool index_ignore_49 = ((index_trunc_46 >= segment_data_dim) || (index_trunc_46 < 0));
			bool index_ignore_50 = ((index_trunc_48 >= segment_data_dim) || (index_trunc_48 < 0));
			// samples segment_data channel 1;
			int chan_51 = m_play_segment_8;
			bool chan_ignore_52 = ((chan_51 < 0) || (chan_51 >= segment_data_channels));
			double read_segment_data_53 = ((chan_ignore_52 || index_ignore_49) ? 0 : m_segment_data_22.read(index_trunc_46, chan_51));
			double read_segment_data_54 = ((chan_ignore_52 || index_ignore_50) ? 0 : m_segment_data_22.read(index_trunc_48, chan_51));
			double readinterp_55 = linear_interp(index_fract_47, read_segment_data_53, read_segment_data_54);
			t_sample expr_431 = readinterp_55;
			if ((m_play_index_7 >= (m_play_len_6 * floor(repeats)))) {
				m_play_index_7 = actual_play_index;
				if ((!m_hold_15)) {
					if ((m_playmode_14 < ((int)1))) {
						m_play_segment_8 = wrap((m_play_segment_8 + ((int)1)), ((int)0), num_segments);
						if ((m_write_segment_11 == m_play_segment_8)) {
							m_play_segment_8 = wrap((m_write_segment_11 + ((int)1)), ((int)0), num_segments);
							
						};
						
					} else {
						if ((m_playmode_14 < ((int)2))) {
							m_play_segment_8 = wrap((m_play_segment_8 - ((int)1)), ((int)0), num_segments);
							if ((m_write_segment_11 == m_play_segment_8)) {
								m_play_segment_8 = wrap((m_write_segment_11 - ((int)1)), ((int)0), num_segments);
								
							};
							
						} else {
							if ((m_playmode_14 < ((int)3))) {
								t_sample sign_56 = noise();
								t_sample direction = ((sign_56 > 0) ? 1 : ((sign_56 < 0) ? -1 : 0));
								m_play_segment_8 = wrap((m_play_segment_8 + direction), ((int)0), num_segments);
								if ((m_write_segment_11 == m_play_segment_8)) {
									m_play_segment_8 = wrap((m_write_segment_11 + direction), ((int)0), num_segments);
									
								};
								
							} else {
								if ((m_playmode_14 < ((int)4))) {
									t_sample direction = (((int)1) + ceil(((num_segments * (noise() + ((int)1))) * ((t_sample)0.5))));
									m_play_segment_8 = wrap((m_play_segment_8 + direction), ((int)0), num_segments);
									if ((m_write_segment_11 == m_play_segment_8)) {
										m_play_segment_8 = wrap((m_write_segment_11 - ((int)1)), ((int)0), num_segments);
										
									};
									
								} else {
									m_play_segment_8 = wrap((m_write_segment_11 - ((int)1)), ((int)0), num_segments);
									
								};
								
							};
							
						};
						
					};
					int length_data_dim = m_length_data_21.dim;
					int length_data_channels = m_length_data_21.channels;
					int index_trunc_57 = fixnan(floor(m_play_segment_8));
					bool index_ignore_58 = ((index_trunc_57 >= length_data_dim) || (index_trunc_57 < 0));
					// samples length_data channel 1;
					m_play_len_6 = (index_ignore_58 ? 0 : m_length_data_21.read(index_trunc_57, 0));
					int offset_data_dim = m_offset_data_20.dim;
					int offset_data_channels = m_offset_data_20.channels;
					int index_trunc_59 = fixnan(floor(m_play_segment_8));
					bool index_ignore_60 = ((index_trunc_59 >= offset_data_dim) || (index_trunc_59 < 0));
					// samples offset_data channel 1;
					m_play_offset_5 = (index_ignore_60 ? 0 : m_offset_data_20.read(index_trunc_59, 0));
					int rms_data_dim = m_rms_data_19.dim;
					int rms_data_channels = m_rms_data_19.channels;
					int index_trunc_61 = fixnan(floor(m_play_segment_8));
					bool index_ignore_62 = ((index_trunc_61 >= rms_data_dim) || (index_trunc_61 < 0));
					// samples rms_data channel 1;
					m_play_rms_4 = (index_ignore_62 ? 0 : m_rms_data_19.read(index_trunc_61, 0));
					
				};
				
			};
			t_sample out1 = expr_431;
			// assign results to output buffer;
			(*(__out1++)) = out1;
			
		};
		return __exception;
		
	};
	inline void set_freq(t_param _value) {
		m_freq_12 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	inline void set_pitchedmode(t_param _value) {
		m_pitchedmode_13 = (_value < 0 ? 0 : (_value > 4 ? 4 : _value));
	};
	inline void set_playmode(t_param _value) {
		m_playmode_14 = (_value < 0 ? 0 : (_value > 4 ? 4 : _value));
	};
	inline void set_hold(t_param _value) {
		m_hold_15 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	inline void set_min_length(t_param _value) {
		m_min_length_16 = (_value < 16 ? 16 : (_value > 10000 ? 10000 : _value));
	};
	inline void set_max_length(t_param _value) {
		m_max_length_17 = (_value < 16 ? 16 : (_value > 10000 ? 10000 : _value));
	};
	inline void set_capture(t_param _value) {
		m_capture_18 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	inline void set_rms_data(void * _value) {
		m_rms_data_19.setbuffer(_value);
	};
	inline void set_offset_data(void * _value) {
		m_offset_data_20.setbuffer(_value);
	};
	inline void set_length_data(void * _value) {
		m_length_data_21.setbuffer(_value);
	};
	inline void set_segment_data(void * _value) {
		m_segment_data_22.setbuffer(_value);
	};
	inline void set_knob(t_param _value) {
		m_knob_23 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	
} State;


///
///	Configuration for the genlib API
///

/// Number of signal inputs and outputs

int gen_kernel_numins = 1;
int gen_kernel_numouts = 1;

int num_inputs() { return gen_kernel_numins; }
int num_outputs() { return gen_kernel_numouts; }
int num_params() { return 12; }

/// Assistive lables for the signal inputs and outputs

const char *gen_kernel_innames[] = { "source to sample" };
const char *gen_kernel_outnames[] = { "out1" };

/// Invoke the signal process of a State object

int perform(CommonState *cself, t_sample **ins, long numins, t_sample **outs, long numouts, long n) {
	State* self = (State *)cself;
	return self->perform(ins, outs, n);
}

/// Reset all parameters and stateful operators of a State object

void reset(CommonState *cself) {
	State* self = (State *)cself;
	self->reset(cself->sr, cself->vs);
}

/// Set a parameter of a State object

void setparameter(CommonState *cself, long index, t_param value, void *ref) {
	State *self = (State *)cself;
	switch (index) {
		case 0: self->set_capture(value); break;
		case 1: self->set_freq(value); break;
		case 2: self->set_hold(value); break;
		case 3: self->set_knob(value); break;
		case 4: self->set_length_data(ref); break;
		case 5: self->set_max_length(value); break;
		case 6: self->set_min_length(value); break;
		case 7: self->set_offset_data(ref); break;
		case 8: self->set_pitchedmode(value); break;
		case 9: self->set_playmode(value); break;
		case 10: self->set_rms_data(ref); break;
		case 11: self->set_segment_data(ref); break;
		
		default: break;
	}
}

/// Get the value of a parameter of a State object

void getparameter(CommonState *cself, long index, t_param *value) {
	State *self = (State *)cself;
	switch (index) {
		case 0: *value = self->m_capture_18; break;
		case 1: *value = self->m_freq_12; break;
		case 2: *value = self->m_hold_15; break;
		case 3: *value = self->m_knob_23; break;
		
		case 5: *value = self->m_max_length_17; break;
		case 6: *value = self->m_min_length_16; break;
		
		case 8: *value = self->m_pitchedmode_13; break;
		case 9: *value = self->m_playmode_14; break;
		
		
		
		default: break;
	}
}

/// Get the name of a parameter of a State object

const char *getparametername(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].name;
	}
	return 0;
}

/// Get the minimum value of a parameter of a State object

t_param getparametermin(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].outputmin;
	}
	return 0;
}

/// Get the maximum value of a parameter of a State object

t_param getparametermax(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].outputmax;
	}
	return 0;
}

/// Get parameter of a State object has a minimum and maximum value

char getparameterhasminmax(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].hasminmax;
	}
	return 0;
}

/// Get the units of a parameter of a State object

const char *getparameterunits(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].units;
	}
	return 0;
}

/// Get the size of the state of all parameters of a State object

size_t getstatesize(CommonState *cself) {
	return genlib_getstatesize(cself, &getparameter);
}

/// Get the state of all parameters of a State object

short getstate(CommonState *cself, char *state) {
	return genlib_getstate(cself, state, &getparameter);
}

/// set the state of all parameters of a State object

short setstate(CommonState *cself, const char *state) {
	return genlib_setstate(cself, state, &setparameter);
}

/// Allocate and configure a new State object and it's internal CommonState:

void *create(t_param sr, long vs) {
	State *self = new State;
	self->reset(sr, vs);
	ParamInfo *pi;
	self->__commonstate.inputnames = gen_kernel_innames;
	self->__commonstate.outputnames = gen_kernel_outnames;
	self->__commonstate.numins = gen_kernel_numins;
	self->__commonstate.numouts = gen_kernel_numouts;
	self->__commonstate.sr = sr;
	self->__commonstate.vs = vs;
	self->__commonstate.params = (ParamInfo *)genlib_sysmem_newptr(12 * sizeof(ParamInfo));
	self->__commonstate.numparams = 12;
	// initialize parameter 0 ("m_capture_18")
	pi = self->__commonstate.params + 0;
	pi->name = "capture";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_capture_18;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 1 ("m_freq_12")
	pi = self->__commonstate.params + 1;
	pi->name = "freq";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_freq_12;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 2 ("m_hold_15")
	pi = self->__commonstate.params + 2;
	pi->name = "hold";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_hold_15;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 3 ("m_knob_23")
	pi = self->__commonstate.params + 3;
	pi->name = "knob";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_knob_23;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 4 ("m_length_data_21")
	pi = self->__commonstate.params + 4;
	pi->name = "length_data";
	pi->paramtype = GENLIB_PARAMTYPE_SYM;
	pi->defaultvalue = 0.;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = false;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 5 ("m_max_length_17")
	pi = self->__commonstate.params + 5;
	pi->name = "max_length";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_max_length_17;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 16;
	pi->outputmax = 10000;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 6 ("m_min_length_16")
	pi = self->__commonstate.params + 6;
	pi->name = "min_length";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_min_length_16;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 16;
	pi->outputmax = 10000;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 7 ("m_offset_data_20")
	pi = self->__commonstate.params + 7;
	pi->name = "offset_data";
	pi->paramtype = GENLIB_PARAMTYPE_SYM;
	pi->defaultvalue = 0.;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = false;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 8 ("m_pitchedmode_13")
	pi = self->__commonstate.params + 8;
	pi->name = "pitchedmode";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_pitchedmode_13;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 4;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 9 ("m_playmode_14")
	pi = self->__commonstate.params + 9;
	pi->name = "playmode";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_playmode_14;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 4;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 10 ("m_rms_data_19")
	pi = self->__commonstate.params + 10;
	pi->name = "rms_data";
	pi->paramtype = GENLIB_PARAMTYPE_SYM;
	pi->defaultvalue = 0.;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = false;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 11 ("m_segment_data_22")
	pi = self->__commonstate.params + 11;
	pi->name = "segment_data";
	pi->paramtype = GENLIB_PARAMTYPE_SYM;
	pi->defaultvalue = 0.;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = false;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	
	return self;
}

/// Release all resources and memory used by a State object:

void destroy(CommonState *cself) {
	State *self = (State *)cself;
	genlib_sysmem_freeptr(cself->params);
		
	delete self;
}


} // GenChopper::
