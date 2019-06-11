" Vim configuration
if has("syntax")
syntax on				" 语法高亮
endif
colorscheme ron				" 颜色配置方案
set cindent				" 使用 C/C++ 语言的自动缩进方式
set tabstop=8				" 设置制表符(tab键)的宽度
set softtabstop=8			" 设置软制表符的宽度
set shiftwidth=8			" (自动) 缩进使用的4个空格
set cinoptions={0,1s,t0,n-2,p2s,(03s,=.5s,>1s,=1s,:1s "设置C/C++语言的具体缩进方式
set number				" 显示行号
set foldmethod=syntax			" 用语法高亮来定义折叠
set foldlevel=100			" 超过100行才使用自动折叠
set foldcolumn=0			" 设置折叠栏宽度
hi ColorColumn ctermbg=LightCyan guibg=LightCyan
set cc=81				" 第81列高亮，警示每行代码不超过80列

"-- Taglist setting --
let Tlist_Ctags_Cmd='ctags'		" 因为我们放在环境变量里，所以可以直接执行
let Tlist_Use_Right_Window=0		" 让窗口显示在右边，0的话就是显示在左边
let Tlist_Show_One_File=0		" 让taglist可以同时展示多个文件的函数列表
let Tlist_File_Fold_Auto_Close=1	" 非当前文件，函数列表折叠隐藏
let Tlist_Exit_OnlyWindow=1		" 当taglist是最后一个分割窗口时，自动推出vim
let Tlist_Process_File_Always=1		" 实时更新tags
let Tlist_Inc_Winwidth=0

"-- WinManager setting --
let g:winManagerWindowLayout='FileExplorer|TagList'	" 设置我们要管理的插件
"let g:persistentBehaviour=0		" 如果所有编辑文件都关闭了，退出vim
nmap <F8> :WMToggle<cr>

" -- MiniBufferExplorer --
let g:miniBufExplMapWindowNavVim = 1	" 按下Ctrl+h/j/k/l，可以切换到当前窗口的上下左右窗口
let g:miniBufExplMapWindowNavArrows = 1	" 按下Ctrl+箭头，可以切换到当前窗口的上下左右窗口
let g:miniBufExplModSelTarget = 1	" 不要在不可编辑内容的窗口（如TagList窗口）中打开选中的buffer

highlight WhitespaceEOL ctermbg=red guibg=red
match WhitespaceEOL /\s\+$/		" 高亮行尾空格
