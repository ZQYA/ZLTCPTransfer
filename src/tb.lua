local tb_meta = {}
tb_meta.__tostring = function(t) 
	local str = {}
	for k in pairs(t) do 
		table.insert(str,k)
		table.insert(str,":")
		table.insert(str,t[k])
		table.insert(str,"\n")
	end 
	return table.concat(str) 
end
tb = {}
tb.new = function(t) 
	if getmetatable(t)~=tb_meta then
		setmetatable(t,tb_meta);
	end
	return t
end
