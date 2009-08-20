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
  groups["swarm2"].color_off = 3
  
  g1 = groups["swarm1"]
  g2 = groups["swarm2"]
  
  g1.rendertype = 0
  
  setBackgroundColor(0, 1, 0)
  
--   require "python"
--   python.execute("from PIL import Image")
--   Image = python.globals().Image
-- --   Image = python.import("PIL").Image
-- 
--   im = Image.open("images/spaceball.gif")
--   width = im.size[0]
--   height = im.size[1]
--   pix = im.load()
  require "limlib2"
  -- im = imlib2.image.load("images/2770641032_34bcf67e9b.jpg")

end

function update()
--   g1.rendertype = 0
--   g2.rendertype = 0
--   g1.speed = 0.2
--   g2.speed = 0.2
  if keys[SDLK_x] then
--     im = imlib2.image.load("images/sun.jpg")
--     // TODO: autoscale
--     height = im:get_height()
--     i = 0
--     print("height", height)
--     for x=1,im:get_width(),9 do
--       for y=1,height,10 do
--         p = g1:get_particle(i)
--         p.pos.x = x/10-20
--         p.pos.y = y/10-20
--         p.pos.z = 0
--         color = im:get_pixel(x,y)
--         p.r = color.red/255
--         p.g = color.green/255
--         p.b = color.blue/255
--         i = i + 1
--       end
--     end
  elseif keys[SDLK_b] then
    setBackgroundColor(0, 0, 0)
  else
    -- groups["swarm1"].speed = 2
    -- groups["swarm2"].color_off = 2
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
		if value == 1 then
			setBackgroundColor(1, 0, 0)
		else
		  setBackgroundColor(0, 0, 0)
		end
  end

--   if value == 0 then
--     return
--   end
  
  swarm_name = path:sub(2,7)
  path = path:sub(8,path:len())
  print()
  print("", swarm_name, path)
  print(path, value)
  print()
  
  g = groups[swarm_name]
  g.speed = 0.1
	
  if path == "/mute" then
    if value > 0.5 then
      g.render = true
    else
      g.render = false
    end
  elseif path == "/notes/bass" then
    if value < 200 then
       g.stay_in_bounds_weight = 1000
       g.vel_render_scale_y = 0.5
       g.vel_render_scale_x = 0.5
       --  g.gridsize = 3
       g.rendertype = 1
       g.speed = 0.1
      --g.color_off = 2
      a = g.pos.z * 0.001
    else
       g.stay_in_bounds_weight = 1000
       g.vel_render_scale_y = 0.5
       g.vel_render_scale_x = 0.5
-- --      size = 4
       g.rendertype = 1
       -- g.gridsize = 7
       g.gridsize = 1
--      g.color_off = value + 1
    end
    g.color_off = g.color_off - 1
  elseif path == "/notes/mid" then
  g.speed = value / 100
    if value < 100 then
      -- g.stay_in_bounds_weight = 100
    else
      -- g.stay_in_bounds_weight = 100000
    end
		g.color_off = g.color_off - 0.0125
  elseif path == "/notes/high" then
    -- g.speed = math.pow(value / 100, 1.3)
		-- g.speed = math.pow(value / 100, 1.3)
--     g.move_to_neighbor_center_weight = 0.2
    g.gridsize = 1
  elseif path == "/amp/drone1" then
    g.color_off = value
  end  
end

-- function doNow()
-- 
-- end









