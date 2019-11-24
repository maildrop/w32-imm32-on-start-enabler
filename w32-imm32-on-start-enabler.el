;;; w32-imm32-on-start-enabler.el --- Package that solves the problem that IMM32 does not work immediately after starting due to an Emacs bug

;; Copyright (C) 2019 by TOGURO Mikito

;; Author: TOGURO Mikito <mit@shalab.net>
;; URL: https://github.com/maildrop/w32-imm32-on-start-enabler
;; Versio: 0.01
;; Package-Requires: w32-imm32-on-start-enabler-implement

;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.

;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

;;; Commentary:


;;; Code:

(when (and (eq system-type 'windows-nt)
	   (locate-library "w32-imm32-on-start-enabler-impl") )
  (load "w32-imm32-on-start-enabler-impl"))

(defvar w32-imm32-on-start-enabler-injected nil
  "In Windows, if it was injected a message hook that sends DispatchMessage() to Emacs GUI thread, set not nil, otherwise nil.")

(defun w32-imm32-on-start-enabler-inject ()
  "In Windows, introduce a message hook that sends DispatchMessage() to Emacs GUI thread when HWND is NULL"
  (when (eq system-type 'windows-nt)
    (unless w32-imm32-on-start-enabler-injected
      (if window-system
          (let ((threadid (w32-imm32-on-start-enabler-impl-hwnd-to-threadid (string-to-number (frame-parameter (selected-frame) 'window-id)))))
	    (when (and threadid (not (eq threadid 0)))
              (setq w32-imm32-on-start-enabler-injected
                    (w32_imm32_on_start_enabler_impl_inject threadid))))
        (setq w32-imm32-on-start-enabler-injected t)))))

(defun w32-imm32-on-start-enabler-deinject ()
  "On Windows, remove the message hook that sends DispatchMessage () to the Emacs GUI thread if HWND is NULL"
  (when (eq system-type 'windows-nt)
    (when w32-imm32-on-start-enabler-injected
      ;; TODO ここで dynamic module を外す
      (setq w32-imm32-on-start-enabler-injected nil))))

(provide 'w32-imm32-on-start-enabler)
;;; w32-imm32-on-start-enabler.el end here
