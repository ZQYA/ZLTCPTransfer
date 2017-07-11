#!/bin/lua
--------------------------------------------------------------------------------
-- this program check the file is a valid jpg or png file 
--------------------------------------------------------------------------------

local function print_binary(str,size) 
	for i = 1,size do 
		local v = str:byte(i)
		print(string.format("%02X",v))
	end
end

local function binary_num(str,size) 
	local t = {}
	for i = 1,size do 
		t[#t+1] = string.format("%02X",str:byte(i))
	end 
	local v = table.concat(t,"")
	return v; 
end

--------------------------------------------------------------------------------
-- check jpg file format use the first and last 2 char to judge the format and 
-- the completion
--------------------------------------------------------------------------------
local function checkjpg(filepath) 
	local f = assert(io.open(filepath,"rb"))
	local pbytes = f:read(2)
	if not pbytes then
		return false 
	elseif pbytes:byte(1) == 0xff and pbytes:byte(2) == 0xd8 then 
		f:seek("end",-2)
		local sbytes = f:read(2)
		print(sbytes:byte(1))
		print (sbytes:byte(2));
		if not sbytes then
			return false 
		elseif sbytes:byte(1) == 0xff and sbytes:byte(2) == 0xd9 then
			return true
		else 
			return false
		end
	else 
		return false 
	end
end

--------------------------------------------------------------------------------
-- check png file format use thr first and last chars accoriding the stander to 
-- judge the format and completeion
--------------------------------------------------------------------------------
local function checkpng(filepath) 
	local f = assert(io.open(filepath,"rb"))
	local pbytes = f:read(8)
	--print_binary(pbytes,8)
	if not pbytes then 
		return false
	elseif binary_num(pbytes,8) == "89504E470D0A1A0A" then 
		f:seek("end",-12)
		local sbytes = f:read(12)
	--	print_binary(sbytes,12)
		if not sbytes then
			return false
		elseif binary_num(sbytes,12) == "0000000049454E44AE426082"then
			return true
		end
	else 
		print("rssd")
		return false
	end
end

--------------------------------------------------------------------------------
-- test code for jpg format 
--------------------------------------------------------------------------------
--local result = checkjpg("/Users/quanzhou/oscode/ZLTCPTransfer/src/3.jpg")
--print(result)

--------------------------------------------------------------------------------
-- test code for png format
--------------------------------------------------------------------------------
--local result = checkpng("3.png")
--print(result)

--------------------------------------------------------------------------------
-- check is a file is a img
--------------------------------------------------------------------------------
local function checkimg(filepath) 
	if checkjpg(filepath) then
		return true
	elseif checkpng(filepath) then
		return true 
	else 
		return false
	end
end

