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

#include "result_blend.h"
#include <tuple>

#include <mf/filter/filter_job.h>
#include <mf/io/image_export.h>


namespace vs {

using namespace mf;

std::pair<real, real> result_blend_filter::weights_(job_type& job) const {
	const auto& left_cam = job.param(left_source_camera);
	const auto& right_cam = job.param(right_source_camera);
	const auto& virtual_cam = job.param(virtual_camera);

	real left_distance = (left_cam.absolute_pose().position - virtual_cam.absolute_pose().position).norm();
	real right_distance = (right_cam.absolute_pose().position - virtual_cam.absolute_pose().position).norm();
	real sum = left_distance + right_distance;
	
	return { left_distance / sum, right_distance / sum };
}


void result_blend_filter::setup() {
	Assert(left_image_input.frame_shape() == right_image_input.frame_shape());
	shape_ = left_image_input.frame_shape();
	virtual_image_output.define_frame_shape(shape_);
	virtual_mask_output.define_frame_shape(shape_);
}


void result_blend_filter::process(job_type& job) {	
	auto virtual_out = job.out(virtual_image_output);
	auto virtual_out_mask = job.out(virtual_mask_output);
	auto left_image_in = job.in(left_image_input);
	auto left_depth_in = job.in(left_depth_input);
	auto left_mask_in = job.in(left_mask_input);
	auto right_image_in = job.in(right_image_input);
	auto right_depth_in = job.in(right_depth_input);
	auto right_mask_in = job.in(right_mask_input);
	
	real left_weight, right_weight;
	std::tie(left_weight, right_weight) = weights_(job);	
	bool prefer_left = (left_weight >= right_weight);
	bool prefer_right = (right_weight >= left_weight);
	
	for(auto coord : make_ndspan(shape_)) {
		color_type left_color = left_image_in.at(coord);
		real_depth_type left_depth = left_depth_in.at(coord);
		tri_mask_type left_mask = left_mask_in.at(coord);

		color_type right_color = right_image_in.at(coord);
		real_depth_type right_depth = right_depth_in.at(coord);
		tri_mask_type right_mask = right_mask_in.at(coord);
	
		color_type virtual_color;
		mask_type virtual_mask = mask_set;

		if(left_mask == tri_mask_clear && right_mask == tri_mask_clear) {
			virtual_mask = mask_clear;
		} else if(left_mask == tri_mask_clear || (left_mask == tri_mask_unstable && prefer_right)) {
			virtual_color = right_color;
		} else if(right_mask == tri_mask_clear || (right_mask == tri_mask_unstable && prefer_left)) {
			virtual_color = left_color;
		} else {
			bool has_depth = (left_mask == tri_mask_stable && right_mask == tri_mask_stable);
			real depth_difference = std::abs(left_depth - right_depth);
			if(has_depth && depth_blending && depth_difference > depth_blending_minimal_difference) {
				if(left_depth < right_depth) virtual_color = left_color;
				else virtual_color = right_color;
			} else {
				virtual_color = color_blend(left_color, left_weight, right_color, right_weight);
			}
		}
		
		virtual_out.at(coord) = virtual_color;
		virtual_out_mask.at(coord) = virtual_mask;
	}
}


}
