require "sdlkeys"

function init()
  groups = {}
  
  groups["swarm1"] = Group:new()
  groups["swarm2"] = Group:new()
  -- groups["swarm3"] = Group:new()

  -- enable when actually connected to OSC source
  --groups["swarm1"].render = false
  --groups["swarm2"].render = false
  --groups["swarm3"].render = false
  
  groups["swarm1"].pos.x = 0
  groups["swarm1"].color_off = 1
  
  groups["swarm2"].pos.x = 0
  groups["swarm2"].color_off = 2
  
  g1 = groups["swarm1"]
  g2 = groups["swarm2"]
  
  setBackgroundColor(0, 1, 0)
end

function update()
  if keys[SDLK_x] then
    groups["swarm1"].speed = 10
    groups["swarm2"].color_off = 2
    setBackgroundColor(0, 0, 1)
  else
    groups["swarm1"].speed = 2
    groups["swarm2"].color_off = 2
    setBackgroundColor(0, 0, 0)
  end
  -- groups["swarm3"].color_off = gettime() - 1247877921.5635
  -- print(" ", gettime())
end

function OSCevent(path, value)
  -- TODO: how does this gsub really work?
--   print("[[[")
--   print(string.gsub(path, "/(swarm[%d]+)/%s", "%1 %2"))
--   print("]]]")
  if path == "/click" then
    setBackgroundColor(1, 0, 0)
  end

  if value == 0 then
    return
  end
  
  swarm_name = path:sub(2,7)
  path = path:sub(8,path:len())
  print()
  print("", swarm_name, path)
  print(path, value)
  print()
  
  g = groups[swarm_name]
  if path == "/mute" then
    if value > 0.5 then
      g.render = true
    else
      g.render = false
    end
  elseif path == "/notes/bass" then
    if value < 400 then
      g.stay_in_bounds_weight = -0.001
      g.vel_render_scale_y = 0.1
      g.vel_render_scale_x = 1
      size = 1
      g.rendertype = 1
      -- g.gridsize = 3
      g.speed = 2
      --g.color_off = 2
      g.color_off = value + 2.1
    else
      g.stay_in_bounds_weight = 0.1
      g.vel_render_scale_y = 1
      g.vel_render_scale_x = 0.1
      size = 4
      g.rendertype = 2
      -- g.gridsize = value / 100
      g.speed = 8
      g.color_off = value + 1
      value = value / 10
    end
    g.gridsize = value / 130
  elseif path == "/notes/mid" then
    g.render_base_size_x = value / 500 / size
    g.render_base_size_y = value / 500 / size
  elseif path == "/notes/high" then
    if value < 0 then
      value = 0
    end
    g.speed = math.pow(value / 100, 1.3)
    g.move_to_neighbor_center_weight = value / 1000
  elseif path == "/amp/drone1" then
    g.color_off = value
  end  
end

-- function doNow()
-- 
-- end









