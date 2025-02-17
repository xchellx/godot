#ifndef RASTERIZER_STORAGE_GX_H
#define RASTERIZER_STORAGE_GX_H

#include "servers/visual/rasterizer.h"
#include "core/image.h"

#include <gccore.h>

class TexCacheRegion : public Reference {
	GDCLASS(TexCacheRegion, Reference);

	public:
	enum TexCacheSize {
		TEXCACHE_32K = GX_TEXCACHE_32K,
		TEXCACHE_128K = GX_TEXCACHE_128K,
		TEXCACHE_512K = GX_TEXCACHE_512K,
		TEXCACHE_NONE = GX_TEXCACHE_NONE,
	};
	private:

	bool _is_initialized;

	GXTexRegion region;
	u32 tmem_even;
	u32 tmem_odd;
	TexCacheSize even_size;
	TexCacheSize odd_size;
	bool is_mipmap;

	public:

	void initialize(bool is_mipmap, u32 tmem_even, TexCacheSize size_even, u32 tmem_odd, TexCacheSize size_odd);
	TexCacheSize get_even_size() const;
	u32 get_even_pos() const;
	TexCacheSize get_odd_size() const;
	u32 get_odd_pos() const;
	bool is_mipmap_cache() const;

	void invalidate_cache();
	GXTexRegion *get_region();

	TexCacheRegion();
};

// TODO: Dynamic texture region allocator?

class RasterizerStorageGX : public RasterizerStorage {
public:
	/* TEXTURE API */
	struct Texture : public RID_Data {
		GXTexObj tex_obj; // GX Texture object description
		Ref<Image> image; // GX Texture data
		uint32_t flags;
		String path;
	};

	struct GXSurface {
		uint32_t format;
		VS::PrimitiveType primitive;
		PoolVector<uint8_t> array;
		int vertex_count;
		PoolVector<uint8_t> index_array;
		int index_count;
		AABB aabb;
		Vector<PoolVector<uint8_t> > blend_shapes;
		Vector<AABB> bone_aabbs;
	};

	struct GXMesh : public RID_Data {
		Vector<GXSurface> surfaces;
		int blend_shape_count;
		VS::BlendShapeMode blend_shape_mode;
	};

	struct RenderTarget : public RID_Data {
		RID texture;
	};

	RenderTarget *current_rt;

	mutable RID_Owner<Texture> texture_owner;
	mutable RID_Owner<GXMesh> mesh_owner;

	RID texture_create();
	void texture_allocate(RID p_texture, int p_width, int p_height, int p_depth_3d, Image::Format p_format, VisualServer::TextureType p_type = VS::TEXTURE_TYPE_2D, uint32_t p_flags = VS::TEXTURE_FLAGS_DEFAULT);
	void texture_set_data(RID p_texture, const Ref<Image> &p_image, int p_level);
	void texture_set_data_partial(RID p_texture, const Ref<Image> &p_image, int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int p_dst_mip, int p_level);
	Ref<Image> texture_get_data(RID p_texture, int p_level) const;
	void texture_set_flags(RID p_texture, uint32_t p_flags);
	uint32_t texture_get_flags(RID p_texture) const;
	Image::Format texture_get_format(RID p_texture) const;
	static void _setup_texture_regions();

	VisualServer::TextureType texture_get_type(RID p_texture) const { return VS::TEXTURE_TYPE_2D; }
	uint32_t texture_get_texid(RID p_texture) const { return 0; }
	uint32_t texture_get_width(RID p_texture) const { return 0; }
	uint32_t texture_get_height(RID p_texture) const { return 0; }
	uint32_t texture_get_depth(RID p_texture) const { return 0; }
	void texture_set_size_override(RID p_texture, int p_width, int p_height, int p_depth_3d) {}
	void texture_bind(RID p_texture, uint32_t p_texture_no) {}

	void texture_set_path(RID p_texture, const String &p_path);
	String texture_get_path(RID p_texture) const;

	void texture_set_shrink_all_x2_on_set_data(bool p_enable) {}

	void texture_debug_usage(List<VS::TextureInfo> *r_info) {}

	RID texture_create_radiance_cubemap(RID p_source, int p_resolution = -1) const { return RID(); }

	void texture_set_detect_3d_callback(RID p_texture, VisualServer::TextureDetectCallback p_callback, void *p_userdata) {}
	void texture_set_detect_srgb_callback(RID p_texture, VisualServer::TextureDetectCallback p_callback, void *p_userdata) {}
	void texture_set_detect_normal_callback(RID p_texture, VisualServer::TextureDetectCallback p_callback, void *p_userdata) {}

	void textures_keep_original(bool p_enable) {}

	void texture_set_proxy(RID p_proxy, RID p_base) {}
	virtual Size2 texture_size_with_proxy(RID p_texture) const { return Size2(); }
	void texture_set_force_redraw_if_visible(RID p_texture, bool p_enable) {}

	/* SKY API */

	RID sky_create() { return RID(); }
	void sky_set_texture(RID p_sky, RID p_cube_map, int p_radiance_size) {}

	/* SHADER API */

	RID shader_create() { return RID(); }

	void shader_set_code(RID p_shader, const String &p_code) {}
	String shader_get_code(RID p_shader) const { return ""; }
	void shader_get_param_list(RID p_shader, List<PropertyInfo> *p_param_list) const {}

	void shader_set_default_texture_param(RID p_shader, const StringName &p_name, RID p_texture) {}
	RID shader_get_default_texture_param(RID p_shader, const StringName &p_name) const { return RID(); }

	void shader_add_custom_define(RID p_shader, const String &p_define) {}
	void shader_get_custom_defines(RID p_shader, Vector<String> *p_defines) const {}
	void shader_clear_custom_defines(RID p_shader) {}

	/* COMMON MATERIAL API */

	RID material_create() { return RID(); }

	void material_set_render_priority(RID p_material, int priority) {}
	void material_set_shader(RID p_shader_material, RID p_shader) {}
	RID material_get_shader(RID p_shader_material) const { return RID(); }

	void material_set_param(RID p_material, const StringName &p_param, const Variant &p_value) {}
	Variant material_get_param(RID p_material, const StringName &p_param) const { return Variant(); }
	Variant material_get_param_default(RID p_material, const StringName &p_param) const { return Variant(); }

	void material_set_line_width(RID p_material, float p_width) {}

	void material_set_next_pass(RID p_material, RID p_next_material) {}

	bool material_is_animated(RID p_material) { return false; }
	bool material_casts_shadows(RID p_material) { return false; }

	void material_add_instance_owner(RID p_material, RasterizerScene::InstanceBase *p_instance) {}
	void material_remove_instance_owner(RID p_material, RasterizerScene::InstanceBase *p_instance) {}

	/* MESH API */

	RID mesh_create() {
		GXMesh *mesh = memnew(GXMesh);
		ERR_FAIL_COND_V(!mesh, RID());
		mesh->blend_shape_count = 0;
		mesh->blend_shape_mode = VS::BLEND_SHAPE_MODE_NORMALIZED;
		return mesh_owner.make_rid(mesh);
	}

	void mesh_add_surface(RID p_mesh, uint32_t p_format, VS::PrimitiveType p_primitive, const PoolVector<uint8_t> &p_array, int p_vertex_count, const PoolVector<uint8_t> &p_index_array, int p_index_count, const AABB &p_aabb, const Vector<PoolVector<uint8_t> > &p_blend_shapes = Vector<PoolVector<uint8_t> >(), const Vector<AABB> &p_bone_aabbs = Vector<AABB>()) {
		GXMesh *m = mesh_owner.getornull(p_mesh);
		ERR_FAIL_COND(!m);

		m->surfaces.push_back(GXSurface());
		GXSurface *s = &m->surfaces.write[m->surfaces.size() - 1];
		s->format = p_format;
		s->primitive = p_primitive;
		s->array = p_array;
		s->vertex_count = p_vertex_count;
		s->index_array = p_index_array;
		s->index_count = p_index_count;
		s->aabb = p_aabb;
		s->blend_shapes = p_blend_shapes;
		s->bone_aabbs = p_bone_aabbs;
	}

	void mesh_set_blend_shape_count(RID p_mesh, int p_amount) {
		GXMesh *m = mesh_owner.getornull(p_mesh);
		ERR_FAIL_COND(!m);
		m->blend_shape_count = p_amount;
	}
	int mesh_get_blend_shape_count(RID p_mesh) const {
		GXMesh *m = mesh_owner.getornull(p_mesh);
		ERR_FAIL_COND_V(!m, 0);
		return m->blend_shape_count;
	}

	void mesh_set_blend_shape_mode(RID p_mesh, VS::BlendShapeMode p_mode) {
		GXMesh *m = mesh_owner.getornull(p_mesh);
		ERR_FAIL_COND(!m);
		m->blend_shape_mode = p_mode;
	}
	VS::BlendShapeMode mesh_get_blend_shape_mode(RID p_mesh) const {
		GXMesh *m = mesh_owner.getornull(p_mesh);
		ERR_FAIL_COND_V(!m, VS::BLEND_SHAPE_MODE_NORMALIZED);
		return m->blend_shape_mode;
	}

	void mesh_surface_update_region(RID p_mesh, int p_surface, int p_offset, const PoolVector<uint8_t> &p_data) {}

	void mesh_surface_set_material(RID p_mesh, int p_surface, RID p_material) {}
	RID mesh_surface_get_material(RID p_mesh, int p_surface) const { return RID(); }

	int mesh_surface_get_array_len(RID p_mesh, int p_surface) const {
		GXMesh *m = mesh_owner.getornull(p_mesh);
		ERR_FAIL_COND_V(!m, 0);

		return m->surfaces[p_surface].vertex_count;
	}
	int mesh_surface_get_array_index_len(RID p_mesh, int p_surface) const {
		GXMesh *m = mesh_owner.getornull(p_mesh);
		ERR_FAIL_COND_V(!m, 0);

		return m->surfaces[p_surface].index_count;
	}

	PoolVector<uint8_t> mesh_surface_get_array(RID p_mesh, int p_surface) const {
		GXMesh *m = mesh_owner.getornull(p_mesh);
		ERR_FAIL_COND_V(!m, PoolVector<uint8_t>());

		return m->surfaces[p_surface].array;
	}
	PoolVector<uint8_t> mesh_surface_get_index_array(RID p_mesh, int p_surface) const {
		GXMesh *m = mesh_owner.getornull(p_mesh);
		ERR_FAIL_COND_V(!m, PoolVector<uint8_t>());

		return m->surfaces[p_surface].index_array;
	}

	uint32_t mesh_surface_get_format(RID p_mesh, int p_surface) const {
		GXMesh *m = mesh_owner.getornull(p_mesh);
		ERR_FAIL_COND_V(!m, 0);

		return m->surfaces[p_surface].format;
	}
	VS::PrimitiveType mesh_surface_get_primitive_type(RID p_mesh, int p_surface) const {
		GXMesh *m = mesh_owner.getornull(p_mesh);
		ERR_FAIL_COND_V(!m, VS::PRIMITIVE_POINTS);

		return m->surfaces[p_surface].primitive;
	}

	AABB mesh_surface_get_aabb(RID p_mesh, int p_surface) const {
		GXMesh *m = mesh_owner.getornull(p_mesh);
		ERR_FAIL_COND_V(!m, AABB());

		return m->surfaces[p_surface].aabb;
	}
	Vector<PoolVector<uint8_t> > mesh_surface_get_blend_shapes(RID p_mesh, int p_surface) const {
		GXMesh *m = mesh_owner.getornull(p_mesh);
		ERR_FAIL_COND_V(!m, Vector<PoolVector<uint8_t> >());

		return m->surfaces[p_surface].blend_shapes;
	}
	Vector<AABB> mesh_surface_get_skeleton_aabb(RID p_mesh, int p_surface) const {
		GXMesh *m = mesh_owner.getornull(p_mesh);
		ERR_FAIL_COND_V(!m, Vector<AABB>());

		return m->surfaces[p_surface].bone_aabbs;
	}

	void mesh_remove_surface(RID p_mesh, int p_index) {
		GXMesh *m = mesh_owner.getornull(p_mesh);
		ERR_FAIL_COND(!m);
		ERR_FAIL_COND(p_index >= m->surfaces.size());

		m->surfaces.remove(p_index);
	}
	int mesh_get_surface_count(RID p_mesh) const {
		GXMesh *m = mesh_owner.getornull(p_mesh);
		ERR_FAIL_COND_V(!m, 0);
		return m->surfaces.size();
	}

	void mesh_set_custom_aabb(RID p_mesh, const AABB &p_aabb) {}
	AABB mesh_get_custom_aabb(RID p_mesh) const { return AABB(); }

	AABB mesh_get_aabb(RID p_mesh, RID p_skeleton) const { return AABB(); }
	void mesh_clear(RID p_mesh) {}

	/* MULTIMESH API */

	virtual RID multimesh_create() { return RID(); }

	void multimesh_allocate(RID p_multimesh, int p_instances, VS::MultimeshTransformFormat p_transform_format, VS::MultimeshColorFormat p_color_format, VS::MultimeshCustomDataFormat p_data = VS::MULTIMESH_CUSTOM_DATA_NONE) {}
	int multimesh_get_instance_count(RID p_multimesh) const { return 0; }

	void multimesh_set_mesh(RID p_multimesh, RID p_mesh) {}
	void multimesh_instance_set_transform(RID p_multimesh, int p_index, const Transform &p_transform) {}
	void multimesh_instance_set_transform_2d(RID p_multimesh, int p_index, const Transform2D &p_transform) {}
	void multimesh_instance_set_color(RID p_multimesh, int p_index, const Color &p_color) {}
	void multimesh_instance_set_custom_data(RID p_multimesh, int p_index, const Color &p_color) {}

	RID multimesh_get_mesh(RID p_multimesh) const { return RID(); }

	Transform multimesh_instance_get_transform(RID p_multimesh, int p_index) const { return Transform(); }
	Transform2D multimesh_instance_get_transform_2d(RID p_multimesh, int p_index) const { return Transform2D(); }
	Color multimesh_instance_get_color(RID p_multimesh, int p_index) const { return Color(); }
	Color multimesh_instance_get_custom_data(RID p_multimesh, int p_index) const { return Color(); }

	void multimesh_set_as_bulk_array(RID p_multimesh, const PoolVector<float> &p_array) {}

	void multimesh_set_visible_instances(RID p_multimesh, int p_visible) {}
	int multimesh_get_visible_instances(RID p_multimesh) const { return 0; }

	AABB multimesh_get_aabb(RID p_multimesh) const { return AABB(); }

	/* IMMEDIATE API */

	RID immediate_create() { return RID(); }
	void immediate_begin(RID p_immediate, VS::PrimitiveType p_rimitive, RID p_texture = RID()) {}
	void immediate_vertex(RID p_immediate, const Vector3 &p_vertex) {}
	void immediate_normal(RID p_immediate, const Vector3 &p_normal) {}
	void immediate_tangent(RID p_immediate, const Plane &p_tangent) {}
	void immediate_color(RID p_immediate, const Color &p_color) {}
	void immediate_uv(RID p_immediate, const Vector2 &tex_uv) {}
	void immediate_uv2(RID p_immediate, const Vector2 &tex_uv) {}
	void immediate_end(RID p_immediate) {}
	void immediate_clear(RID p_immediate) {}
	void immediate_set_material(RID p_immediate, RID p_material) {}
	RID immediate_get_material(RID p_immediate) const { return RID(); }
	AABB immediate_get_aabb(RID p_immediate) const { return AABB(); }

	/* SKELETON API */

	RID skeleton_create() { return RID(); }
	void skeleton_allocate(RID p_skeleton, int p_bones, bool p_2d_skeleton = false) {}
	void skeleton_set_base_transform_2d(RID p_skeleton, const Transform2D &p_base_transform) {}
	void skeleton_set_world_transform(RID p_skeleton, bool p_enable, const Transform &p_world_transform) {}
	int skeleton_get_bone_count(RID p_skeleton) const { return 0; }
	void skeleton_bone_set_transform(RID p_skeleton, int p_bone, const Transform &p_transform) {}
	Transform skeleton_bone_get_transform(RID p_skeleton, int p_bone) const { return Transform(); }
	void skeleton_bone_set_transform_2d(RID p_skeleton, int p_bone, const Transform2D &p_transform) {}
	Transform2D skeleton_bone_get_transform_2d(RID p_skeleton, int p_bone) const { return Transform2D(); }

	/* Light API */

	RID light_create(VS::LightType p_type) { return RID(); }

	RID directional_light_create() { return light_create(VS::LIGHT_DIRECTIONAL); }
	RID omni_light_create() { return light_create(VS::LIGHT_OMNI); }
	RID spot_light_create() { return light_create(VS::LIGHT_SPOT); }

	void light_set_color(RID p_light, const Color &p_color) {}
	void light_set_param(RID p_light, VS::LightParam p_param, float p_value) {}
	void light_set_shadow(RID p_light, bool p_enabled) {}
	void light_set_shadow_color(RID p_light, const Color &p_color) {}
	void light_set_projector(RID p_light, RID p_texture) {}
	void light_set_negative(RID p_light, bool p_enable) {}
	void light_set_cull_mask(RID p_light, uint32_t p_mask) {}
	void light_set_reverse_cull_face_mode(RID p_light, bool p_enabled) {}
	void light_set_use_gi(RID p_light, bool p_enabled) {}

	void light_omni_set_shadow_mode(RID p_light, VS::LightOmniShadowMode p_mode) {}
	void light_omni_set_shadow_detail(RID p_light, VS::LightOmniShadowDetail p_detail) {}

	void light_directional_set_shadow_mode(RID p_light, VS::LightDirectionalShadowMode p_mode) {}
	void light_directional_set_blend_splits(RID p_light, bool p_enable) {}
	bool light_directional_get_blend_splits(RID p_light) const { return false; }
	void light_directional_set_shadow_depth_range_mode(RID p_light, VS::LightDirectionalShadowDepthRangeMode p_range_mode) {}
	VS::LightDirectionalShadowDepthRangeMode light_directional_get_shadow_depth_range_mode(RID p_light) const { return VS::LIGHT_DIRECTIONAL_SHADOW_DEPTH_RANGE_STABLE; }

	VS::LightDirectionalShadowMode light_directional_get_shadow_mode(RID p_light) { return VS::LIGHT_DIRECTIONAL_SHADOW_ORTHOGONAL; }
	VS::LightOmniShadowMode light_omni_get_shadow_mode(RID p_light) { return VS::LIGHT_OMNI_SHADOW_DUAL_PARABOLOID; }

	bool light_has_shadow(RID p_light) const { return false; }

	VS::LightType light_get_type(RID p_light) const { return VS::LIGHT_OMNI; }
	AABB light_get_aabb(RID p_light) const { return AABB(); }
	float light_get_param(RID p_light, VS::LightParam p_param) { return 0.0; }
	Color light_get_color(RID p_light) { return Color(); }
	bool light_get_use_gi(RID p_light) { return false; }
	uint64_t light_get_version(RID p_light) const { return 0; }

	/* PROBE API */

	RID reflection_probe_create() { return RID(); }

	void reflection_probe_set_update_mode(RID p_probe, VS::ReflectionProbeUpdateMode p_mode) {}
	void reflection_probe_set_intensity(RID p_probe, float p_intensity) {}
	void reflection_probe_set_interior_ambient(RID p_probe, const Color &p_ambient) {}
	void reflection_probe_set_interior_ambient_energy(RID p_probe, float p_energy) {}
	void reflection_probe_set_interior_ambient_probe_contribution(RID p_probe, float p_contrib) {}
	void reflection_probe_set_max_distance(RID p_probe, float p_distance) {}
	void reflection_probe_set_extents(RID p_probe, const Vector3 &p_extents) {}
	void reflection_probe_set_origin_offset(RID p_probe, const Vector3 &p_offset) {}
	void reflection_probe_set_as_interior(RID p_probe, bool p_enable) {}
	void reflection_probe_set_enable_box_projection(RID p_probe, bool p_enable) {}
	void reflection_probe_set_enable_shadows(RID p_probe, bool p_enable) {}
	void reflection_probe_set_cull_mask(RID p_probe, uint32_t p_layers) {}
	void reflection_probe_set_resolution(RID p_probe, int p_resolution) {}

	AABB reflection_probe_get_aabb(RID p_probe) const { return AABB(); }
	VS::ReflectionProbeUpdateMode reflection_probe_get_update_mode(RID p_probe) const { return VisualServer::REFLECTION_PROBE_UPDATE_ONCE; }
	uint32_t reflection_probe_get_cull_mask(RID p_probe) const { return 0; }
	Vector3 reflection_probe_get_extents(RID p_probe) const { return Vector3(); }
	Vector3 reflection_probe_get_origin_offset(RID p_probe) const { return Vector3(); }
	float reflection_probe_get_origin_max_distance(RID p_probe) const { return 0.0; }
	bool reflection_probe_renders_shadows(RID p_probe) const { return false; }

	void instance_add_skeleton(RID p_skeleton, RasterizerScene::InstanceBase *p_instance) {}
	void instance_remove_skeleton(RID p_skeleton, RasterizerScene::InstanceBase *p_instance) {}

	void instance_add_dependency(RID p_base, RasterizerScene::InstanceBase *p_instance) {}
	void instance_remove_dependency(RID p_base, RasterizerScene::InstanceBase *p_instance) {}

	/* GI PROBE API */

	RID gi_probe_create() { return RID(); }

	void gi_probe_set_bounds(RID p_probe, const AABB &p_bounds) {}
	AABB gi_probe_get_bounds(RID p_probe) const { return AABB(); }

	void gi_probe_set_cell_size(RID p_probe, float p_range) {}
	float gi_probe_get_cell_size(RID p_probe) const { return 0.0; }

	void gi_probe_set_to_cell_xform(RID p_probe, const Transform &p_xform) {}
	Transform gi_probe_get_to_cell_xform(RID p_probe) const { return Transform(); }

	void gi_probe_set_dynamic_data(RID p_probe, const PoolVector<int> &p_data) {}
	PoolVector<int> gi_probe_get_dynamic_data(RID p_probe) const {
		PoolVector<int> p;
		return p;
	}

	void gi_probe_set_dynamic_range(RID p_probe, int p_range) {}
	int gi_probe_get_dynamic_range(RID p_probe) const { return 0; }

	void gi_probe_set_energy(RID p_probe, float p_range) {}
	float gi_probe_get_energy(RID p_probe) const { return 0.0; }

	void gi_probe_set_bias(RID p_probe, float p_range) {}
	float gi_probe_get_bias(RID p_probe) const { return 0.0; }

	void gi_probe_set_normal_bias(RID p_probe, float p_range) {}
	float gi_probe_get_normal_bias(RID p_probe) const { return 0.0; }

	void gi_probe_set_propagation(RID p_probe, float p_range) {}
	float gi_probe_get_propagation(RID p_probe) const { return 0.0; }

	void gi_probe_set_interior(RID p_probe, bool p_enable) {}
	bool gi_probe_is_interior(RID p_probe) const { return false; }

	void gi_probe_set_compress(RID p_probe, bool p_enable) {}
	bool gi_probe_is_compressed(RID p_probe) const { return false; }

	uint32_t gi_probe_get_version(RID p_probe) { return 0; }

	GIProbeCompression gi_probe_get_dynamic_data_get_preferred_compression() const { return GI_PROBE_UNCOMPRESSED; }
	RID gi_probe_dynamic_data_create(int p_width, int p_height, int p_depth, GIProbeCompression p_compression) { return RID(); }
	void gi_probe_dynamic_data_update(RID p_gi_probe_data, int p_depth_slice, int p_slice_count, int p_mipmap, const void *p_data) {}

	/* LIGHTMAP CAPTURE */
	struct Instantiable : public RID_Data {

		SelfList<RasterizerScene::InstanceBase>::List instance_list;

		_FORCE_INLINE_ void instance_change_notify(bool p_aabb = true, bool p_materials = true) {

			SelfList<RasterizerScene::InstanceBase> *instances = instance_list.first();
			while (instances) {

				instances->self()->base_changed(p_aabb, p_materials);
				instances = instances->next();
			}
		}

		_FORCE_INLINE_ void instance_remove_deps() {
			SelfList<RasterizerScene::InstanceBase> *instances = instance_list.first();
			while (instances) {

				SelfList<RasterizerScene::InstanceBase> *next = instances->next();
				instances->self()->base_removed();
				instances = next;
			}
		}

		Instantiable() {}
		virtual ~Instantiable() {
		}
	};

	struct LightmapCapture : public Instantiable {

		PoolVector<LightmapCaptureOctree> octree;
		AABB bounds;
		Transform cell_xform;
		int cell_subdiv;
		float energy;
		LightmapCapture() {
			energy = 1.0;
			cell_subdiv = 1;
		}
	};

	mutable RID_Owner<LightmapCapture> lightmap_capture_data_owner;
	void lightmap_capture_set_bounds(RID p_capture, const AABB &p_bounds) {}
	AABB lightmap_capture_get_bounds(RID p_capture) const { return AABB(); }
	void lightmap_capture_set_octree(RID p_capture, const PoolVector<uint8_t> &p_octree) {}
	RID lightmap_capture_create() {
		LightmapCapture *capture = memnew(LightmapCapture);
		return lightmap_capture_data_owner.make_rid(capture);
	}
	PoolVector<uint8_t> lightmap_capture_get_octree(RID p_capture) const {
		const LightmapCapture *capture = lightmap_capture_data_owner.getornull(p_capture);
		ERR_FAIL_COND_V(!capture, PoolVector<uint8_t>());
		return PoolVector<uint8_t>();
	}
	void lightmap_capture_set_octree_cell_transform(RID p_capture, const Transform &p_xform) {}
	Transform lightmap_capture_get_octree_cell_transform(RID p_capture) const { return Transform(); }
	void lightmap_capture_set_octree_cell_subdiv(RID p_capture, int p_subdiv) {}
	int lightmap_capture_get_octree_cell_subdiv(RID p_capture) const { return 0; }
	void lightmap_capture_set_energy(RID p_capture, float p_energy) {}
	float lightmap_capture_get_energy(RID p_capture) const { return 0.0; }
	const PoolVector<LightmapCaptureOctree> *lightmap_capture_get_octree_ptr(RID p_capture) const {
		const LightmapCapture *capture = lightmap_capture_data_owner.getornull(p_capture);
		ERR_FAIL_COND_V(!capture, NULL);
		return &capture->octree;
	}

	/* PARTICLES */

	RID particles_create() { return RID(); }

	void particles_set_emitting(RID p_particles, bool p_emitting) {}
	void particles_set_amount(RID p_particles, int p_amount) {}
	void particles_set_lifetime(RID p_particles, float p_lifetime) {}
	void particles_set_one_shot(RID p_particles, bool p_one_shot) {}
	void particles_set_pre_process_time(RID p_particles, float p_time) {}
	void particles_set_explosiveness_ratio(RID p_particles, float p_ratio) {}
	void particles_set_randomness_ratio(RID p_particles, float p_ratio) {}
	void particles_set_custom_aabb(RID p_particles, const AABB &p_aabb) {}
	void particles_set_speed_scale(RID p_particles, float p_scale) {}
	void particles_set_use_local_coordinates(RID p_particles, bool p_enable) {}
	void particles_set_process_material(RID p_particles, RID p_material) {}
	void particles_set_fixed_fps(RID p_particles, int p_fps) {}
	void particles_set_fractional_delta(RID p_particles, bool p_enable) {}
	void particles_restart(RID p_particles) {}

	void particles_set_draw_order(RID p_particles, VS::ParticlesDrawOrder p_order) {}

	void particles_set_draw_passes(RID p_particles, int p_count) {}
	void particles_set_draw_pass_mesh(RID p_particles, int p_pass, RID p_mesh) {}

	void particles_request_process(RID p_particles) {}
	AABB particles_get_current_aabb(RID p_particles) { return AABB(); }
	AABB particles_get_aabb(RID p_particles) const { return AABB(); }

	void particles_set_emission_transform(RID p_particles, const Transform &p_transform) {}

	bool particles_get_emitting(RID p_particles) { return false; }
	int particles_get_draw_passes(RID p_particles) const { return 0; }
	RID particles_get_draw_pass_mesh(RID p_particles, int p_pass) const { return RID(); }

	virtual bool particles_is_inactive(RID p_particles) const { return false; }

	/* RENDER TARGET */

	mutable RID_Owner<RenderTarget> render_target_owner;

	RID render_target_create();
	void render_target_set_position(RID p_render_target, int p_x, int p_y) {}
	void render_target_set_size(RID p_render_target, int p_width, int p_height);
	RID render_target_get_texture(RID p_render_target) const;
	void render_target_set_external_texture(RID p_render_target, unsigned int p_texture_id) {}
	void render_target_set_flag(RID p_render_target, RenderTargetFlags p_flag, bool p_value) {}
	bool render_target_was_used(RID p_render_target) { return false; }
	void render_target_clear_used(RID p_render_target) {}
	void render_target_set_msaa(RID p_render_target, VS::ViewportMSAA p_msaa) {}

	/* CANVAS SHADOW */

	RID canvas_light_shadow_buffer_create(int p_width) { return RID(); }

	/* LIGHT SHADOW MAPPING */

	RID canvas_light_occluder_create() { return RID(); }
	void canvas_light_occluder_set_polylines(RID p_occluder, const PoolVector<Vector2> &p_lines) {}

	VS::InstanceType get_base_type(RID p_rid) const {
		if (mesh_owner.owns(p_rid)) {
			return VS::INSTANCE_MESH;
		}

		return VS::INSTANCE_NONE;
	}

	bool free(RID p_rid) {
		if (texture_owner.owns(p_rid)) {
			// delete the texture
			Texture *texture = texture_owner.get(p_rid);
			texture_owner.free(p_rid);
			memdelete(texture);
		} else if (mesh_owner.owns(p_rid)) {
			// delete the mesh
			GXMesh *mesh = mesh_owner.getornull(p_rid);
			mesh_owner.free(p_rid);
			memdelete(mesh);
		} else if (lightmap_capture_data_owner.owns(p_rid)) {
			// delete the lightmap
			LightmapCapture *lightmap_capture = lightmap_capture_data_owner.getornull(p_rid);
			lightmap_capture_data_owner.free(p_rid);
			memdelete(lightmap_capture);
		} else {
			return false;
		}

		return true;
	}

	bool has_os_feature(const String &p_feature) const { return false; }

	void update_dirty_resources() {}

	void set_debug_generate_wireframes(bool p_generate) {}

	void render_info_begin_capture() {}
	void render_info_end_capture() {}
	int get_captured_render_info(VS::RenderInfo p_info) { return 0; }

	int get_render_info(VS::RenderInfo p_info) { return 0; }
	String get_video_adapter_name() const { return String(); }
	String get_video_adapter_vendor() const { return String(); }

	static RasterizerStorage *base_singleton;

	RasterizerStorageGX() {};
	~RasterizerStorageGX() {}
};

#endif