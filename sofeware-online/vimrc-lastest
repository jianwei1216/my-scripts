"------------------------vundle管理所有的插件begin--------------------------
set fileencodings=ucs-bom,utf-8,cp936
let mapleader=","
set nocompatible
filetype off
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()

"Plugin 'VundleVim/Vundle.vim'
Plugin 'gmarik/Vundle.vim'

"Plugin 'bronson/vim-update-bundles'

"Plugin 'scrooloose/nerdtree'
Plugin 'vim-scripts/The-NERD-tree'

Plugin 'vim-scripts/The-NERD-Commenter'

"Plugin 'Valloric/YouCompleteMe'

Plugin 'vim-scripts/taglist.vim'

"Plugin 'vim-scripts/SuperTab'
"Plugin 'Lokaltog/vim-powerline.git'
"Plugin 'Lokaltog/powerline-fonts.git'

Plugin 'vim-scripts/c.vim'

Plugin 'vim-scripts/cscope_plus.vim'
"Plugin 'vim-scripts/JumpInCode-new-update'
"Plugin 'vim-scripts/JumpInCode'

"Plugin 'xolox/vim-misc'
"Plugin 'xolox/vim-easytags'

Plugin 'vim-scripts/log.vim'

Plugin 'L9'

"自动补全括号
Plugin 'jiangmiao/auto-pairs.git'

call vundle#end()
filetype plugin indent on
"------------------------vundle管理所有的插件end--------------------------
"-------------------------------------------------------------------------------
"
set fileencodings=ucs-bom,utf-8,cp936
set nocompatible
set nu
set cindent
set mouse=a

"Tlist {{{
nnoremap <silent> <script> <F2> <ESC><ESC> : TlistToggle<CR>
"inoremap <silent> <script> <F3> <ESC><ESC> : TlistToggle<CR>
let Tlist_Auto_Highlight_Tag = 1
"let Tlist_Auto_Open = 1
let Tlist_Auto_Update = 1
let Tlist_Close_On_Select = 1
let Tlist_Compact_Format = 1
let Tlist_File_Fold_Auto_Close = 1
"}}}

"NERDTree {{{
nnoremap <silent> <script> <F3> <ESC><ESC> : NERDTreeToggle<CR>
"inoremap <silent> <script> <F5> <ESC><ESC> : NERDTreeToggle<CR>
let NERDTreeWinPos="right"
let NERDTreeShowMarks=1
"}}}

set foldmethod=manual
filetype  plugin on
filetype  indent on
syntax    on            

"-------------------------------------------------------------------------------
" Various settings
"-------------------------------------------------------------------------------
set autoindent                  " copy indent from current line
set autoread                    " read open files again when changed outside Vim
set autowrite                   " write a modified buffer on each :next , ...
set backspace=indent,eol,start  " backspacing over everything in insert mode
set nobackup                      " keep a backup file
set browsedir=current           " which directory to use for the file browser
set complete+=k                 " scan the files given with the 'dictionary' option
set history=50                  " keep 50 lines of command line history
set hlsearch                    " highlight the last used search pattern
set incsearch                   " do incremental searching
set listchars=tab:>.,eol:\$     " strings to use in 'list' mode
set mouse=a                     " enable the use of the mouse
set nowrap                      " do not wrap lines
set popt=left:8pc,right:3pc     " print options
set ruler                       " show the cursor position all the time
set shiftwidth=8               " number of spaces to use for each step of indent
set softtabstop=8              " number of spaces substitue of tabs
set tabstop=8                  " number of spaces that a <Tab> counts for
set expandtab
"set showcmd                     " display incomplete commands
set smartindent                 " smart autoindenting when starting a new line
"set visualbell                  " visual bell instead of beeping
"set wildignore=*.bak,*.o,*.e,*~ " wildmenu: ignore these extensions
"set wildmenu                    " command-line completion in an enhanced mode
"autocmd FileType make set noexpandtab

"color {{{
"hi Normal guibg=#99cc99 guifg=Black
"hi LineNr guibg=#003366 guifg=#99ccff ctermbg=7777 ctermfg=blue
set cursorline
"hi CursorLine cterm=NONE ctermbg=darkred ctermfg=white guibg=#66cc99 guifg=black
"}}}

"StatusLine{{{
set ruler
set laststatus=2
set statusline+=%F%m%r%h%w\ [ASCII=\%03.3b]\ [HEX=\%02.2B]\ [POS=%04l,%04v][%p%%]\ [LEN=%L]\ %{strftime(\"%d/%m/%y-%H:%M\")} 
"set guifont=PowerlineSymbols\ for\ Powerline
"set t_Co=256
"set noshowmode
"let g:Powerline_symbols='fancy'
"let Powerline_symbols='compatible'
"}}}
"
"-------------------------------------------------------------------------------
" close window (conflicts with the KDE setting for calling the process manager)
"-------------------------------------------------------------------------------
 noremap  <C-Esc>       :close<CR>
inoremap  <C-Esc>  <C-C>:close<CR>
"
"-------------------------------------------------------------------------------
" Fast switching between buffers
" The current buffer will be saved before switching to the next one.
" Choose :bprevious or :bnext
"-------------------------------------------------------------------------------
 noremap  <silent> <s-tab>       :if &modifiable && !&readonly && 
     \                      &modified <CR> :write<CR> :endif<CR>:bprevious<CR>
inoremap  <silent> <s-tab>  <C-C>:if &modifiable && !&readonly && 
     \                      &modified <CR> :write<CR> :endif<CR>:bprevious<CR>
"
"-------------------------------------------------------------------------------
" Leave the editor with Ctrl-q (KDE): Write all changed buffers and exit Vim
"-------------------------------------------------------------------------------
nnoremap  <C-q>    :wqall<CR>
"
"-------------------------------------------------------------------------------
" When editing a file, always jump to the last known cursor position.
" Don't do it when the position is invalid or when inside an event handler
" (happens when dropping a file on gvim).
"-------------------------------------------------------------------------------
if has("autocmd")
  autocmd BufReadPost *
        \ if line("'\"") > 0 && line("'\"") <= line("$") |
        \   exe "normal! g`\"" |
        \ endif
endif " has("autocmd")

"

if has("autocmd")
  " ----------  qmake : set filetype for *.pro  ----------
  autocmd BufNewFile,BufRead *.pro  set filetype=qmake
endif " has("autocmd")
let OmniCpp_NamespaceSearch = 1 
let OmniCpp_GlobalScopeSearch = 1 
let OmniCpp_ShowAccess = 1 
let OmniCpp_ShowPrototypeInAbbr = 1 " show function parameters
let OmniCpp_MayCompleteDot = 1 " autocomplete after .
let OmniCpp_MayCompleteArrow = 1 " autocomplete after ->
let OmniCpp_MayCompleteScope = 1 " autocomplete after ::
let OmniCpp_DefaultNamespaces = ["std", "_GLIBCXX_STD"]
au CursorMovedI, InsertLeave * if pumvisible() == 0|silent! pclose|endif
set completeopt=menuone,longest 
colorscheme delek
