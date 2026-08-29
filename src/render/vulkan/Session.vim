let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
doautoall SessionLoadPre
silent only
silent tabonly
cd ~/Dev/cpp/game-dev/varicle/src/render/vulkan
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
let s:shortmess_save = &shortmess
set shortmess+=aoO
badd +1 src/vulkan-lean.hpp
badd +1 src/test.cpp
badd +63 CMakeLists.txt
badd +29 ~/Dev/cpp/test/vulkan_render/main.cpp
badd +20 src/core/init.cpp
badd +5 src/core/context.hpp
badd +1 src/core/init.hpp
badd +4 ~/Dev/cpp/game-dev/varicle/examples/opengl-test/CMakeLists.txt
badd +2 ~/Dev/cpp/game-dev/varicle/examples/opengl-test/src/main.cpp
argglobal
%argdel
edit ~/Dev/cpp/test/vulkan_render/main.cpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
wincmd _ | wincmd |
split
1wincmd k
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe 'vert 1resize ' . ((&columns * 90 + 91) / 182)
exe '2resize ' . ((&lines * 28 + 30) / 61)
exe 'vert 2resize ' . ((&columns * 91 + 91) / 182)
exe '3resize ' . ((&lines * 29 + 30) / 61)
exe 'vert 3resize ' . ((&columns * 91 + 91) / 182)
argglobal
setlocal foldmethod=expr
setlocal foldexpr=v:lua.vim.treesitter.foldexpr()
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=99
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
30
sil! normal! zo
36
sil! normal! zo
40
sil! normal! zo
46
sil! normal! zo
48
sil! normal! zo
115
sil! normal! zo
115
sil! normal! zo
748
sil! normal! zo
787
sil! normal! zo
796
sil! normal! zo
807
sil! normal! zo
828
sil! normal! zo
929
sil! normal! zo
947
sil! normal! zo
968
sil! normal! zo
1022
sil! normal! zo
let s:l = 31 - ((22 * winheight(0) + 28) / 57)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 31
let s:c = 16 - ((10 * winwidth(0) + 45) / 90)
if s:c > 0
  exe 'normal! ' . s:c . '|zs' . 16 . '|'
else
  normal! 016|
endif
wincmd w
argglobal
if bufexists(fnamemodify("src/vulkan-lean.hpp", ":p")) | buffer src/vulkan-lean.hpp | else | edit src/vulkan-lean.hpp | endif
if &buftype ==# 'terminal'
  silent file src/vulkan-lean.hpp
endif
balt src/core/context.hpp
setlocal foldmethod=expr
setlocal foldexpr=v:lua.vim.treesitter.foldexpr()
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=99
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
4
sil! normal! zo
7
sil! normal! zo
let s:l = 46 - ((5 * winheight(0) + 13) / 27)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 46
normal! 02|
wincmd w
argglobal
if bufexists(fnamemodify("src/core/init.cpp", ":p")) | buffer src/core/init.cpp | else | edit src/core/init.cpp | endif
if &buftype ==# 'terminal'
  silent file src/core/init.cpp
endif
balt src/vulkan-lean.hpp
setlocal foldmethod=expr
setlocal foldexpr=v:lua.vim.treesitter.foldexpr()
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=99
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
11
sil! normal! zo
18
sil! normal! zo
38
sil! normal! zo
79
sil! normal! zo
90
sil! normal! zo
107
sil! normal! zo
120
sil! normal! zo
138
sil! normal! zo
147
sil! normal! zo
171
sil! normal! zo
let s:l = 20 - ((13 * winheight(0) + 14) / 28)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 20
normal! 0
wincmd w
3wincmd w
exe 'vert 1resize ' . ((&columns * 90 + 91) / 182)
exe '2resize ' . ((&lines * 28 + 30) / 61)
exe 'vert 2resize ' . ((&columns * 91 + 91) / 182)
exe '3resize ' . ((&lines * 29 + 30) / 61)
exe 'vert 3resize ' . ((&columns * 91 + 91) / 182)
tabnext 1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0 && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20
let &shortmess = s:shortmess_save
let &winminheight = s:save_winminheight
let &winminwidth = s:save_winminwidth
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
set hlsearch
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
