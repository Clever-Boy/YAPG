entity_template = {
    name = "kenney_block_grass_slope_right",
    friendlyname = "[Kenney] Right slope grass block",

    parameters = {
        x = {
            name = "X position",
            component = "position",
            type = "number",
            attribute = "x",
        },
        y = {
            name = "Y position",
            component = "position",
            type = "number",
            attribute = "y",
        },
    },

    components = {
        ["position"] = {
            x = 0,
            y = 0,
            width = 64,
            height = 64
        },
        ["platform"] = {

        },
        ["hitbox"] = {
            polygon = {
                points = {
                    {
                        x = 64,
                        y = 0
                    },
                    {
                        x = 64,
                        y = 64
                    },
                    {
                        x = 0,
                        y = 64
                    },
                }
            }
        },
        ["render"] = {
            texture = "spritesheet_complete.png",
            current_animation = "default",
            animations = {
                ["default"] = {
                    total_duration = 1,
                    frames = {
                        {
                            rect = { left = 1560, top = 650, width = 128, height = 128},
                            relative_duration = 1,
                        },
                    },
                },
            },
        }
    }
}
