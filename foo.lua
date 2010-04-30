require "sdlkeys"

-- group1 : red
-- group2 : blue

-- TODO: 
-- x mute
-- global speed variable
-- more helpful error load messages
-- ? don't repeat keys (keydown instead of keypress)

function init()
  pattern = {}
  groups = {}
  
  groups["swarm1"] = Group:new()
  groups["swarm2"] = Group:new()
--   groups["swarm3"] = Group:new()

  -- enable when actually connected to OSC source
  --groups["swarm1"].render = false
  --groups["swarm2"].render = false
  --groups["swarm3"].render = false
  
  groups["swarm1"].pos.x = 0
  groups["swarm1"].color_off = 1
  groups["swarm1"].swarm_number = 1
  
  groups["swarm2"].pos.x = 0
  groups["swarm2"].color_off = 3
  groups["swarm2"].swarm_number = 2
  
  g1 = groups["swarm1"]
  g2 = groups["swarm2"]
  
  g1.rendertype = 0
  
  swarm = g1
  
  setBackgroundColor(0, 1, 0)
end

function update()
  if keys[SDLK_BACKSLASH] and keys[SDLK_LSHIFT] then
    fn_name = "vel render scale y"
    fn = function (swarm, x)
      swarm.vel_render_scale_y = swarm.vel_render_scale_y * (1 + x * 0.01)
      return swarm.vel_render_scale_y
    end
  elseif keys[SDLK_MINUS] then
    fn_name = "vel render scale x"
    fn = function (swarm, x)
      swarm.vel_render_scale_x = swarm.vel_render_scale_x * (1 + x * 0.01)
      return swarm.vel_render_scale_x
    end
  elseif keys[SDLK_r] then
    fn_name = "radius"
    fn = function (swarm, x)
      swarm.radius = swarm.radius + x * 0.1
      return swarm.radius
    end
  elseif keys[SDLK_o] then
    fn_name = "vel render scale x & y"
    fn = function (swarm, x)
      swarm.vel_render_scale_x = swarm.vel_render_scale_x * (1 + x * 0.01)
      swarm.vel_render_scale_y = swarm.vel_render_scale_y * (1 + x * 0.01)
      return ""..swarm.vel_render_scale_x..','..swarm.vel_render_scale_y
    end
  elseif keys[SDLK_b] then
    fn_name = "render base size x & y"
    fn = function (swarm, x)
      swarm.render_base_size_x = swarm.render_base_size_x * (1 + x * 0.001)
      swarm.render_base_size_y = swarm.render_base_size_y * (1 + x * 0.001)
      return ""..swarm.render_base_size_x..','..swarm.render_base_size_y
    end
  elseif keys[SDLK_w] then
    fn_name = "move to neighbor center weight"
    fn = function (swarm, x)
      swarm.move_to_neighbor_center_weight = swarm.move_to_neighbor_center_weight * (1 + x * 0.01)
      return swarm.move_to_neighbor_center_weight
    end
  elseif keys[SDLK_v] then
    fn_name = "maximum velocity"
    fn = function (swarm, x)
      swarm.maximum_velocity = swarm.maximum_velocity * (1 + x * 0.01)
      return swarm.maximum_velocity
    end
  elseif keys[SDLK_s] then
    fn_name = "stay in bounds weight"
    fn = function (swarm, x)
      swarm.stay_in_bounds_weight = swarm.stay_in_bounds_weight * (1 + x * 0.01)
      return swarm.stay_in_bounds_weight
    end
  elseif keys[SDLK_z] then
    fn_name = "pos.z"
    fn = function (swarm, x)
      swarm.pos.z = swarm.pos.z + x * 0.1
      return swarm.pos.z
    end
  elseif keys[SDLK_y] then
    fn_name = "pos.y"
    fn = function (swarm, x)
      swarm.pos.y = swarm.pos.y + x * 0.1
      return swarm.pos.y
    end
  elseif keys[SDLK_x] then
    fn_name = "pos.x"
    fn = function (swarm, x)
      swarm.pos.x = swarm.pos.x + x * 0.1
      return swarm.pos.x
    end
  elseif keys[SDLK_a] then
    fn_name = "alpha"
    fn = function (swarm, x)
      if swarm.alpha < 0 then
        swarm.alpha = 0
      elseif swarm.alpha >= 1 then
        swarm.alpha = 1
      end
      swarm.alpha = swarm.alpha + x * 0.001
      return swarm.alpha
    end
    print(fn_name)
  elseif keys[SDLK_p] then
    fn_name = "speed"
    fn = function (swarm, x)
      swarm.speed = swarm.speed + x * 0.01
      return swarm.speed
    end
  elseif keys[SDLK_UP] then
    ret = fn(swarm, 1)
    print(swarm.swarm_number, fn_name, ret)
  elseif keys[SDLK_DOWN] then
    ret = fn(swarm, -1)
    print(swarm.swarm_number, fn_name, ret)
  elseif keys[SDLK_PAGEUP] then
    ret = fn(swarm, 10)
    print(swarm.swarm_number, fn_name, ret)
  elseif keys[SDLK_PAGEDOWN] then
    ret = fn(swarm, -10)
    print(swarm.swarm_number, fn_name, ret)
  elseif keys[SDLK_HOME] then
    setBackgroundColor(0, 0, 0)
    for i = 0,500 do
      p1 = g1:get_particle(i)
      p2 = g2:get_particle(i)
      p1.r = 0.75 + math.random() * 0.25
      p1.g = math.random() * 0.5
      p1.b = math.random() * 0.5
      p2.r = math.random() * 0.5
      p2.g = math.random() * 0.5
      p2.b = 0.75 + math.random() * 0.25
    end
  elseif keys[SDLK_1] then
    swarm = groups["swarm1"]
  elseif keys[SDLK_2] then
    swarm = groups["swarm2"]
  elseif keys[SDLK_F1] then
    pattern["swarm0"] = {
      ['fn'] = fn,
      ['swarm'] = swarm,
    }
  elseif keys[SDLK_F2] then
    pattern["swarm1"] = {
      ['fn'] = fn,
      ['swarm'] = swarm,
    }
  else
  end
end

function OSCevent(path, value)
  swarm_name = path:sub(2,7)
  path = path:sub(8,path:len())
  print("", swarm_name, path)
  print(path, value)
  print()
  
  if swarm_name == "swarm0" then
--[[    if pattern['swarm0'] then
      p = pattern['swarm0']
      p['fn'](p['swarm'], )
    end]]
    g1.speed = math.pow(value-1, 1.5)
  elseif swarm_name == "swarm1" then
    g1.vel_render_scale_x = value / 5
    g2.vel_render_scale_y = value / 5
  elseif swarm_name == "swarm2" then
    g2.speed = math.pow(value, 2)
    g2.pos.y = (value - 2) * 10
  elseif swarm_name == "swarm3" then
    g1.gridsize = value
    g1.rendertype = 1
    g2.pos.z = -value*10
  elseif swarm_name == "swarm4" then
    g2.move_to_neighbor_center_weight = math.pow(value, 2)/20
  elseif swarm_name == "swarm5" then
  end  
end

-- function doNow()
-- 
-- end









