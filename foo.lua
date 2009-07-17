require "sdlkeys"

function init()
  groups = {}
  
  groups["swarm1"] = Group:new()
  groups["swarm2"] = Group:new()
  groups["swarm3"] = Group:new()
  
  groups["swarm2"].pos.x = -30
  groups["swarm3"].pos.x = 30
end

function update()
  if keys[SDLK_x] then
    groups["swarm1"].speed = 10
  else
    groups["swarm1"].speed = 2
  end
end

function OSCevent(path, value)
  -- TODO: how does this gsub really work?
  local swarm_name, path = string.gsub(path, "/(swarm[%d]+)/(.*)")
  g = groups[swarm_name]
  if path == "/mute" then
    if value > 0.5 then
      g.render = True
    else
      g.render = False
    end
  elseif path == "/notes/bass" then
    if value < 200 then
      g.stay_in_bounds_weight = -0.001
      g.vel_render_scale_y = 1
      size = 1
    else
      g.stay_in_bounds_weight = 0.01
      g.vel_render_scale_y = 0
      size = 2
    end
  elseif path == "/notes/mid" then
    g.render_base_size_x = value / 250 / size
    g.render_base_size_y = value / 250 / size
  elseif path == "/notes/high" then
    g.speed = math.pow(value / 100, 1.3)
    g.move_to_neighbor_center_weight = value / 1000
  elseif path == "/amp/drone1" then
    g.color_off = value
  end
  
  print(path, value)
end

-- function doNow()
-- 
-- end









