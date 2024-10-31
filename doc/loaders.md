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

## Materials

```json
{
  "frame_buffer": 0,
  
}
```

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
