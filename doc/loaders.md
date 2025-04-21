# Asset structures for loaders

## Properties available for all assets.

```json
{
  "name": "asset_name",
  "use_present": true,
  "save": true,
  "override": false,
  "save_weak": false
}
```

- **name**: the name of the asset. The combination of the name and the asset's type will serve as its identifier. This
  property is required.
- **use_present**: if true, the loader will search for an asset with the same identifier as this asset. If present, the
  loader will use the found asset instead. This property is optional and its default value is `false`.
- **save**: whether the asset will be saved in the asset collection provided to the loader. You can use this property to
  save the asset for future uses. This property is optional and its default value is `false`.
- **override**: whether this asset will override any already present asset with the same identifier in the asset
  collection. This property is optional and its default value is `false`.
- **save_weak**: whether this asset will be saved using the weak storage mode. If true, the asset collection will save a
  weak reference to the asset. This property is optional and its default value is `false`.

## Shaders

```json
{
  "vertex|fragment|geometry|task|mesh": {
    "file": "default.vert",
    "raw": "#version ..."
  }
}
```

Each stage must include a `file` or `raw` parameter. The parameter `file` has priority over the `raw` parameter.

## Materials

```json
{
  "frame_buffer": "A:frame_buffer",
  "shader": "A:shader",
  "descriptions": {
    // Vertex can be an array or an object.
    "vertex": [
      {
        "stride": 8,
        // Attributes can be an array or an object
        "attributes": [
          {
            "size_in_floats": 1,
            "offset_in_bytes": 0
          },
          {
            "size_in_floats": 1,
            "offset_in_bytes": 4
          }
        ]
      }
    ],
    "instance": {
      // Same as vertex.
    },
    // If 'uniform_buffer' is not defined, 'uniform' can be used to describe a new uniform buffer!.
    "uniform": "A:shader_uniform_descriptor",
    "uniform_buffer": "A:shader_uniform_buffer",
    "bindings": [
      {
        "binding": 0,
        "location": "global"
      },
      {
        "binding": 1,
        "location": "material"
      },
      {
        "binding": 2,
        "location": "extra",
        "descriptor": "A:another_shader_uniform_descriptor"
      }
    ]
  },
  "blending": {
    "logic_blending": false,
    "logic_operation": "clear|and|and_reverse|copy|and_inverted|no_op|xor|or|nor|equivalent|invert|or_reverse|copy_inverted|or_inverted|or_inverted|nand|set"
    "blending_constants": [
      0.0,
      1.1,
      2.2,
      3.3
    ],
    "attachments": [
      {
        "enabled": false,
        // Valid parameters: A, R, G or B.
        "write_mask": "ARGB",
        "color_blend_operation": "add|subtract|reverse_subtract|min|max",
        "color_source_blend_factor": "zero|one|src_color|one_minus_src_color|dst_color|one_minus_dst_color|src_alpha|one_minus_src_alpha|dst_alpha|one_minus_dst_alpha|constant_color|one_minus_constant_color|constant_alpha|one_minus_constant_alpha|src_alpha_saturate|src1_color|one_minus_src1_color|src1_alpha|one_minus_src1_alpha",
        "color_destiny_blend_factor": "zero|one|src_color|one_minus_src_color|dst_color|one_minus_dst_color|src_alpha|one_minus_src_alpha|dst_alpha|one_minus_dst_alpha|constant_color|one_minus_constant_color|constant_alpha|one_minus_constant_alpha|src_alpha_saturate|src1_color|one_minus_src1_color|src1_alpha|one_minus_src1_alpha",
        "alpha_blend_operation": "add|subtract|reverse_subtract|min|max",
        "alpha_source_blend_factor": "zero|one|src_color|one_minus_src_color|dst_color|one_minus_dst_color|src_alpha|one_minus_src_alpha|dst_alpha|one_minus_dst_alpha|constant_color|one_minus_constant_color|constant_alpha|one_minus_constant_alpha|src_alpha_saturate|src1_color|one_minus_src1_color|src1_alpha|one_minus_src1_alpha",
        "alpha_destiny_blend_factor": "zero|one|src_color|one_minus_src_color|dst_color|one_minus_dst_color|src_alpha|one_minus_src_alpha|dst_alpha|one_minus_dst_alpha|constant_color|one_minus_constant_color|constant_alpha|one_minus_constant_alpha|src_alpha_saturate|src1_color|one_minus_src1_color|src1_alpha|one_minus_src1_alpha"
      }
    ]
  },
  "depth_stencil": {
    "depth_test": true,
    "depth_write": true,
    "depth_compare_operation": "never|less|equal|less_or_equal|greater|not_equal|grater_or_equal|always",
    "use_depth_bounds": false,
    "min_depth_bounds": 0.0,
    "max_depth_bounds": 1.0
  },
  "rasterizer": {
    "polygon_mode": "fill|line|point|fill_rectangle_nvidia",
    "line_width": 1.0,
    "cull_mode": "none|front|back|both",
    "clockwise": false
  },
  "topology": "point_list|line_list|line_strip|triangle_list|triangle_strip|triangle_fan|line_list_with_adjacency|line_strip_with_adjacency|triangle_list_with_adjacency|triangle_strip_with_adjacency|patch_list",
  "priority": 0
}
```

The only required parameters are `frame_buffer` and `shader`.

The parameter 'descriptor.uniform' describes the uniform buffer that will be created along with the material,
while the parameter 'descriptor.bindings' describes the binding set the defined uniform buffers will be bound to
when the material is used. By default, the global buffer will be bound to the set 0 and the material buffer will be
bound to the set 1.

## Meshes

```json
{
  "modifiable_vertices": false,
  "modifiable_indices": false,
  // Materials can also be a string or an object
  "materials": [
    "A:material"
  ],
  "vertices": [
    0.0,
    1.1,
    2.2,
    3.3
  ],
  "indices": [
    0,
    1,
    2,
    0,
    2,
    3
  ]
}
```

All parameters are optional.

## Assimp scenes

```json
{
  "file": "object.obj",
  "flip_uvs": false,
  "flip_winding_order": false
}
```

Only the parameter `file` is required.

## Models

```json
{
  "maximum_instances": 20000,
  "auto_flush": true,
  "custom_instance_sizes": [
    128
  ],
  "meshes": [
    "A:mesh"
  ],
  "assimp": "A:assimp_scene",
  "assimp_metadata": {
    "materials": [
      // One entry per assimp material. If there are not enough entries,
      // the first entry will be used. This list can also be an object.
      {
        "material": "A:material",
        "textures": {
          // The key is the name of the texture inside the sahder.
          // The value is the texture type loaded by assimp.
          "nameOfMyTextureInsideTheShader": "none|diffuse|specular|ambient|emissive|height|normals|shininess|opacity|displacement|lightmap|reflection|base_color|normal_camera|emission_color"
        }
      }
    ],
    "meshes": [
      // One entry per assimp mesh. If there are not enough entries,
      // the first entry will be used. This list can also be an object.
      {
        // This list is used to determine the order of the elements in the buffer.
        // It must match the input of the shader.
        // You can repeat entries.
        "input": [
          "position",
          "normal",
          "tangent",
          "uv",
          "color"
        ]
      }
    ],
    "extra_materials": [
      "A:extra_material"
    ]
  }
}
```

All parameters are optional, but if you don't include them an empty model will be generated.

## Shader uniform descriptors

```json
{
  // Bindings can also be an object.
  "bindings": [
    {
      "type": "image|storage_buffer|uniform_buffer",
      // Only if type is not image.
      "size": 0
    }
  ]
}
```

## Shader uniform buffers

```json
{
  "descriptor": "A:shader_uniform_descriptor"
}
```

## Textures

```json
{
  // Source can also be an object
  "source": [
    "my/file.png",
    "other/file.jpg"
  ],
  "image": {
    "format": "R8|R8G8|R8G8B8|B8G8R8|R8G8B8A8|R8G8B8A8_SRGB|A8R8G8B8|B8G8R8A8|A8B8G8R8|R32FG32FB32F|R32FG32FB32FA32F|R32F|R16FG16F|R16FG16FB16F|R16FG16FB16FA16F|DEPTH24STENCIL8",
    "dimensions": "ONE|TWO|THREE",
    // If width, height or depth are not defined or are 0, that parameter will be determined by the source.
    "width": 0,
    "height": 0,
    "depth": 0,
    "layers": 1,
    "tiling": "OPTIMAL|LINEAR|DRM",
    "samples": "COUNT_1|COUNT_2|COUNT_4|COUNT_8|COUNT_16|COUNT_32|COUNT_64",
    // If mipmaps are 0, the amount will be calculated automatically.
    "mipmaps": 0,
    "usages": [
      "TRANSFER_SOURCE",
      "TRANSFER_DESTINY",
      "SAMPLING",
      "STORAGE",
      "COLOR_ATTACHMENT",
      "DEPTH_STENCIL_ATTACHMENT",
      "TRANSFER_ATTACHMENT",
      "INPUT_ATTACHMENT",
      "VIDEO_DECODE_DESTINY",
      "VIDEO_DECODE_SOURCE",
      "VIDEO_DECODE_DPB",
      "FRAGMENT_DESTINY_MAP",
      "FRAGMENT_SHADING_RATE_ATTACHMENT",
      "ATTACHMENT_FEEDBACK_LOOP",
      "INVOCATION_MARK",
      "SAMPLE_WEIGHT",
      "SAMPLE_BLOCK_MATCH"
    ]
  },
  "image_view": {
    "view_type": "NORMAL_1D|NORMAL_2D|NORMAL_3D|CUBE|ARRAY_1D|ARRAY_2D|CUBE_ARRAY",
    "r_swizzle": "IDENTITY|ZERO|ONE|R|G|B|A",
    "g_swizzle": "IDENTITY|ZERO|ONE|R|G|B|A",
    "b_swizzle": "IDENTITY|ZERO|ONE|R|G|B|A",
    "a_swizzle": "IDENTITY|ZERO|ONE|R|G|B|A",
    "base_mipmap_level": 0,
    // If mipmap_level_count is 0, the view will use all mipmap levels.
    "mipmap_level_count": 0,
    "mipmap_array_layer_level": 0,
    // If array_layer_count, the view will use all layers.
    "array_layer_count": 0
  },
  "sampler": {
    "magnification_filter": "NEAREST|LINEAR|CUBIC",
    "minification_filter": "NEAREST|LINEAR|CUBIC",
    "u_address_mode": "REPEAT|MIRRORED_REPEAT|CLAMP_TO_EDGE|CLAMP_TO_BORDER|MIRROR_CLAMP_TO_EDGE",
    "v_address_mode": "REPEAT|MIRRORED_REPEAT|CLAMP_TO_EDGE|CLAMP_TO_BORDER|MIRROR_CLAMP_TO_EDGE",
    "w_address_mode": "REPEAT|MIRRORED_REPEAT|CLAMP_TO_EDGE|CLAMP_TO_BORDER|MIRROR_CLAMP_TO_EDGE",
    "anisotropy": true,
    // If -1, the value will be set to max.
    "max_anisotropy": -1,
    "normalized_coordinates": true,
    "mipmap_mode": "NEAREST|LINEAR"
  }
}
```

If no sources are available, the texture won't be created.

## Frame buffers

```json
{
  "type": "swap_chain",
  "depth": false,
  "samples": "COUNT_1|COUNT_2|COUNT_4|COUNT_8|COUNT_16|COUNT_32|COUNT_64"
}
```

```json
{
  "type": "simple",
  "depth": false,
  "samples": "COUNT_1|COUNT_2|COUNT_4|COUNT_8|COUNT_16|COUNT_32|COUNT_64",
  // Textures can also be an object.
  "textures": [
    {
      // General properties are available here!
      "name": "A texture",
      "save": true,
      "format": "r8g8b8a8|...",
      "layers": 1,
      "image_view": {
        // Just like a texture.
      },
      "sampler": {
        // Just like a texture.
      },
      "resolved": {
        // Section only valid if the output uses MSAA.
        // General properties are available here!
        "name": "A resolved texture",
        "save": true
      }
    }
  ],
  "depth_properties": {
    // General properties are available here!
    "name": "Depth texture",
    "save": true
  }
}
```

Only the property `type` is required.

## Render pass strategies

```json
{
  "frame_buffer": "A:frame_buffer",
  "priority": 0
}
```

Only the property `frame_buffer` is required.
Strategies are always `DefaultRenderPassStrategy`.

## Renders

```json
{
  "global_uniform_descriptor": "A:uniform_descriptor",
  //Render_passes can also be an object or a string.
  "render_passes": [
    "A:render_pass_strategies",
    "render/pass.json",
    {
      // Render pass...
    }
  ]
}
```

All render passes will be loaded in order.

## How to fetch children assets

You can load already existing assets using a name, a file, the raw source, or a combination of all three ways.

In the following example, the material's shader will be fetch from the given file.

```json

{
  "name": "My material",
  "shader": "shader.json"
}

```

You can also fetch already created asset by giving its name.

```json

{
  "name": "My material",
  "shader": "A:My Shader"
}

```

You can also append the raw information of the children asset.

```json

{
  "name": "My material",
  "shader": {
    "name": "My Shader",
    ...
  }
}

```

Finally, you can provide multiple ways to fetch the required asset.

```json

{
  "name": "My material",
  "shader": [
    "A:My Shader",
    "shader.json",
    {
      "name": "My Shader",
      ...
    }
  ]
}

```

The fetch algorithm will try to fetch the shader
using the giving providers in order.
When a fetch fails, the algorithm will jump
to the next provider.
