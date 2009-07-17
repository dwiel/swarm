require "sdlkeys"

function init()
  g1 = Group:new()
  g2 = Group:new()
end

function update()
--   if keys[SDLK_x] then
--     g.speed = 10
--   else
--     g.speed = 2
--   end

--  if g2 == nil then
--    g2 = Group:new()
--  end
  g.pos.z = 0
end

function OSCevent(path, value)
  if path == "/notes/bass" then
    if value < 200 then
      g.stay_in_bounds_weight = -0.001
      g.vel_render_scale_y = 1
      size = 1
    else
      g.stay_in_bounds_weight = 0.01
      g.vel_render_scale_y = 0
      size = 2
    end
  end
  if path == "/notes/mid" then
    g.render_base_size_x = value / 250 / size
    g.render_base_size_y = value / 250 / size
  end
  if path == "/notes/high" then
    g.speed = math.pow(value / 100, 1.3)
    g.move_to_neighbor_center_weight = value / 1000
  end
  if path == "/amp/drone1" then
    g.color_off = value
  end
  
  print(path, value)
end

-- TODO
-- function doNow()
-- 
-- end









