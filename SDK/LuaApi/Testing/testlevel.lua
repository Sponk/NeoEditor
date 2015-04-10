return {
  version = "1.1",
  luaversion = "5.1",
  orientation = "orthogonal",
  width = 20,
  height = 10,
  tilewidth = 70,
  tileheight = 70,
  properties = {},
  tilesets = {
    {
      name = "tiles_spritesheet",
      firstgid = 1,
      filename = "../../../../tiles_spritesheet.tsx",
      tilewidth = 70,
      tileheight = 70,
      spacing = 2,
      margin = 0,
      image = "../../maps/Tiles/tiles_spritesheet.png",
      imagewidth = 914,
      imageheight = 936,
      properties = {},
      tiles = {}
    }
  },
  layers = {
    {
      type = "tilelayer",
      name = "Kachelebene 1",
      x = 0,
      y = 0,
      width = 20,
      height = 10,
      visible = true,
      opacity = 1,
      properties = {},
      encoding = "lua",
      data = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 61, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 10, 10, 1, 1, 1, 1, 1, 1, 1, 61, 61, 61, 61, 61, 0,
        61, 61, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 61, 61, 61, 61, 61, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
      }
    },
    {
      type = "objectgroup",
      name = "Objektebene 1",
      visible = true,
      opacity = 1,
      properties = {},
      objects = {
        {
          name = "torch1",
          type = "torch",
          shape = "rectangle",
          x = 933,
          y = 145,
          width = 0,
          height = 0,
          gid = 26,
          visible = true,
          properties = {}
        },
        {
          name = "torch2",
          type = "torch",
          shape = "rectangle",
          x = 1069,
          y = 144,
          width = 0,
          height = 0,
          gid = 38,
          visible = true,
          properties = {}
        }
      }
    }
  }
}
