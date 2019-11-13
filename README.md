# w32-imm32-on-start-enabler
Emacs package 

Emacs 起動直後にMS-IMEが日本語入力できないの多分こういう理由
MS-IME は自身の通知に、hwnd == NULL でメッセージをポストして、DefWindowProc の向こう側で、それをフックしている。
この仕組みに対して、Emacs は w32fns.c のw32_msg_pump() の最後で msg.hwnd == NULL の時のメッセージがDefWindowProc{A,W} には渡していない。
一旦最小化したり、タイトルバーを動かすと日本語入力できるようになるのは、この関数の中で、メッセージポンプが動くため。
キーワードは KiUserCallbackDispatcher 

- How to use
-- コンパイルして、w32-imm32-on-start-enabler.el とw32-imm32-on-start-enabler-impl.dll を
 share\emacs\27.0.50\site-lisp に置く
-- init.el で

(when (and (locate-library "w32-imm32-on-start-enabler" )
	   (load "w32-imm32-on-start-enabler") )
  (w32-imm32-on-start-enabler-inject))

で有効化させる。

 