Object = {}
function Object:new(child)
    child = child or {}
    child.__index = self
    child.parent = self
    setmetatable(child, child)
    return child
end
setmetatable(Object, Object)


Package = Object:new()
GitRepo = Object:new()


function use(getter)
    return getter:get()
end
