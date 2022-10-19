function incoming(text) 
	local verb = text:match("Press (%a+) to continue:")
	if verb ~= nil then
		if verb == 'Return' then
			jmc.send("")
			return 1;
		end
	end
	return 0
end
