local autoMove = {
    directions = {
        "has_north", "has_south", "has_west", "has_east", "has_down", "has_up",
        ["has_north"] = 1, 
        ["has_south"] = 2, 
        ["has_west"] = 3, 
        ["has_east"] = 4, 
        ["has_down"] = 5, 
        ["has_up"] = 6,
        ["North"] = 1, 
        ["South"] = 2, 
        ["West"] = 3, 
        ["East"] = 4, 
        ["Down"] = 5, 
        ["Up"] = 6
   },
    goDirections = {"North", "South", "West", "East", "Down", "Up"},
    dataStore = {},
    get = function(self, var)
        return self.dataStore[var]
    end,
    set = function(self, var, val)
        self.dataStore[var] = val
    end,
    shouldLook = function(self)
        return (tonumber(self:get("look_for_dir")) or 0) == 1
    end,
    stopLoooking = function(self)
        self:set("look_for_dir", 0)
    end,
    startLooking = function(self)
        self:set("look_for_dir", 1)
    end,
    resetDirs = function(self)
        self:stopLoooking()
        for i = 1, #self.directions do
            local dirName = self.directions[i]
            self:set(dirName, 0)
        end
    end,
    isTired = function(self)
        return (tonumber(self:get("is_tired")) or 0) == 1
    end,
    imTired = function(self)
        self:set("is_tired", 1)
    end,
    imNotTired = function(self)
        self:set("is_tired", 0)
    end,
    getHasDirs = function(self)
        local retval = {}
        for i = 1, #self.directions do
            local dirName = self.directions[i]
            if (tonumber(self:get(dirName)) or 0) == 1 then
                table.insert(retval, i)
            end
        end
        return retval
    end,
    enableDirection = function(self, direction)
        if direction ~= nil then
            local idx = self.directions[direction]
            if idx ~= nil then
                self:set(self.directions[idx], 1)
            end
        end
    end
}

function incoming(text)
    if autoMove:isTired() then
        local hp,mana,move = text:match("^|<(%d+)hp (%d+)m (%d+)mv>")
        if (tonumber(move) or 0) > 60 then
            autoMove:imNotTired()
            jmc.send("wake")
            jmc.send("exits")
        end
        return 0
    end

    if text:match("^You are too exhausted") ~= nil then
        autoMove:stopLoooking()
        autoMove:imTired()
        jmc.send("sleep")
        return 0
    end

    if autoMove:shouldLook() then
        if text:match("^|<%d+hp %d+m %d+mv>") ~= nil then
            local mydirs = autoMove:getHasDirs()
            autoMove:resetDirs()
            local randomDirIndex = jmc.getRandom(1, #mydirs)
            local actualDirIndex = mydirs[randomDirIndex]
            local godir = autoMove.goDirections[actualDirIndex]
            if godir ~= nil then
                jmc.send(godir)
                jmc.send("exits")
            else
                jmc.print("go is nil")
            end
        else
            autoMove:enableDirection(text:match("^(%a+) - .+"))
        end
    elseif text:match("^Obvious exits:") then
        autoMove:startLooking()
    end

    return 0
end