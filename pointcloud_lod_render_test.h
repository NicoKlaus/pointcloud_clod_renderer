#pragma once

#include <cgv/base/node.h>
#include <cgv/math/fvec.h>
#include <cgv/media/color.h>
#include <cgv/gui/event_handler.h>
#include <cgv/gui/provider.h>
#include <cgv/data/data_view.h>
#include <cgv/render/drawable.h>
#include <cgv/render/shader_program.h>
#include <cgv/render/texture.h>
#include <cgv/render/render_types.h>
#include <cgv_gl/point_renderer.h>
#include <cgv_gl/line_renderer.h>
#include <cgv_gl/rounded_cone_renderer.h>
#include <cgv_gl/surfel_renderer.h>
#include <cgv_gl/clod_point_renderer.h>

#include <point_cloud.h>
#include <octree.h>

#include <string>
#include <mutex>
#include <future>
#include <unordered_set>

// these are the vr specific headers
#include <vr/vr_driver.h>
#include <cg_vr/vr_server.h>
#include <vr_view_interactor.h>
#include <vr_render_helpers.h>

#include "lib_begin.h"

enum class LoDMode {
	OCTREE = 1,
	RANDOM_POISSON = 2,
	INVALID = -1
};

class pointcloud_lod_render_test :
	public cgv::base::node,
	public cgv::render::drawable,
	public cgv::gui::event_handler,
	public cgv::gui::provider
{
public:
	pointcloud_lod_render_test();

	/// overload to return the type name of this object. By default the type interface is queried over get_type.
	std::string get_type_name() const { return "pointcloud_lod_render_test"; }

	bool self_reflect(cgv::reflect::reflection_handler& rh);

	void on_set(void* member_ptr);
	
	void on_register();
	
	void unregister();

	/// adjust view
	bool init(cgv::render::context& ctx);
	/// overload to draw the content of this drawable
	void draw(cgv::render::context& ctx);
	///
	void find_pointcloud(cgv::render::context& ctx);
	///
	void clear(cgv::render::context& ctx);
	/// 
	bool handle(cgv::gui::event& e);
	/// 
	void stream_help(std::ostream& os);
	///
	void create_gui();

protected:
	void timer_event(double t, double dt);

	void on_load_point_cloud_cb();
	void on_clear_point_cloud_cb();
	void on_rotate_x_cb();
	void rotate_pc_x(const float angle);
	void rotate_pc_y(const float angle);
	void on_rotate_y_cb();
	void on_rotate_z_cb();
	void on_randomize_position_cb();
	void on_reg_find_point_cloud_cb();
	void on_point_cloud_style_cb();
	void on_lod_mode_change();

	void construct_table(float tw, float td, float th, float tW);
	void construct_room(float w, float d, float h, float W, bool walls, bool ceiling);
	void construct_environment(float s, float ew, float ed, float w, float d, float h);
	void build_scene(float w, float d, float h, float W, float tw, float td, float th, float tW);
	void clear_scene();
	void build_test_object_32();


	point_cloud build_test_point_cloud(int x, int y, int z, int grid_size,float cube_size) {
		double dgrid_size = grid_size;
		point_cloud pc;
		pc.create_colors();
		for (int x = 0; x < grid_size; ++x) {
			for (int y = 0; y < grid_size; ++y) {
				for (int z = 0; z < grid_size; ++z) {
					double dx = x, dy = y, dz = z;
					int i = std::max(std::max(x, y), z)+1;
					vec3 v(dx, dy, dz);
					v *= cube_size / grid_size;
					while (i > 0) {
						pc.add_point(v);
						pc.clr(pc.get_nr_points() - 1) = rgb8(i);
						--i;
					}
				}
			}
		}
		return std::move(pc);
	}

private:
	std::unordered_set<void*> rebuild_ptrs;

	std::string ply_path;
	point_cloud source_pc, crs_srs_pc;
	cgv::render::point_render_style source_prs;
	cgv::render::surfel_render_style source_srs;
	cgv::render::line_render_style lrs;
	cgv::render::rounded_cone_render_style rcrs;

	size_t max_points = -1;

	float rot_intensity;
	float trans_intensity;
	bool view_find_point_cloud;
	bool renderer_out_of_date = true;
	bool recolor_point_cloud = true;
	
	bool pointcloud_fit_table = true;
	bool put_on_table = true;
	bool color_based_on_lod = false;
	bool show_environment = true;

	static constexpr float min_level_hue = 230.0/360.0;
	static constexpr float max_level_hue = 1.0;
	
	int lod_mode = (int)LoDMode::OCTREE;
	
	bool gui_model_positioning = false;
	vec3 model_position= vec3(0);
	vec3 model_rotation = vec3(0);
	float model_scale = 1.f;

	cgv::render::clod_point_render_style cp_style;

	std::vector<box3> boxes;
	std::vector<rgb> box_colors;
	cgv::render::box_render_style style;
	constexpr static float table_height = 0.7f;

	octree_lod_generator lod_generator;
	std::vector<LODPoint> points_with_lod;
};

#include <cgv/config/lib_end.h>