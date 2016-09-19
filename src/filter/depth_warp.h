/*
Author : Tim Lenertz
Date : May 2016

Copyright (c) 2016, Université libre de Bruxelles

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files to deal in the Software without restriction, including the rights to use, copy, modify, merge,
publish the Software, and to permit persons to whom the Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef VW_FILTER_DEPTH_WARP_H_
#define VW_FILTER_DEPTH_WARP_H_

#include <mf/filter/filter.h>
#include <mf/filter/filter_parameter.h>
#include "../common.h"

namespace vs {

class depth_warp_filter : public mf::flow::filter {
public:
	input_type<2, integral_depth_type> depth_input;
	output_type<2, real_depth_type> depth_output;
	output_type<2, mask_type> depth_mask_output;
	
	parameter_type<camera_type> source_camera;
	parameter_type<camera_type> destination_camera;
	
	depth_warp_filter() :
		depth_input(*this),
		depth_output(*this),
		depth_mask_output(*this),
		source_camera(*this),
		destination_camera(*this)
	{
		depth_input.set_name("di");
		depth_output.set_name("di");
		depth_mask_output.set_name("di mask");
		source_camera.set_name("source cam");
		destination_camera.set_name("dest cam");
	}

	void setup() override;
	void process(job_type& job) override;	
};
	
}

#endif
