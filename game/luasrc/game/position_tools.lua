do
    local P = {}

    -- Tools to test the relative position of objects after a collision
    position_tools = P

    P.was_over = function(entity, other)
        if(position(entity) == nil or position(other) == nil or hitbox(entity) == nil or hitbox(other) == nil) then
            return false
        end

        return
        ((position(entity).old_y + hitbox(entity).polygon:get_local_bounding_box().top + hitbox(entity).polygon:get_local_bounding_box().height)
        <=  (position(other).old_y + hitbox(other).polygon:get_local_bounding_box().top))
    end

    P.was_left_to = function(entity, other)
        --TODO
    end

    P.was_right_to = function(entity, other)
        --TODO
    end

    P.was_under = function(entity, other)
        --TODO
    end
end
