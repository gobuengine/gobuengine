-- local requires = {}
-- Engine.Component("Floor", {
--     Position
-- })
local phaser = {OnPhysics = 1, OnUpdate = 2, onDrawing = 4, OnDrawingUI = 6}

Engine.System(phaser.OnUpdate, function(it)
    local render = Engine.ecs_field(it, 2)
    local count = Engine.ecs_iter_count(it)

    for i = count, 1, 1
    do
        local rotation = Engine.get_rotate(render, i)
        Engine.set_rotate(render, i, rotation + 0.5)
    end

end)

