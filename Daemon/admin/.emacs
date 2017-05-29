;;; Emacs file: David Sandel
;;; Used with Emacs 24.3


(package-initialize)
(setq load-path (cons (expand-file-name "~/.emacs.d/bin/") load-path))


; CC Mode
(defun dcs-c-common-hook()


; Hungrily delete whitespace
(c-toggle-auto-hungry-state 1))
(add-hook 'c-mode-common-hook 'dcs-c-common-hook)


; Remove all trailing whitespace before saving
(add-hook 'before-save-hook 'delete-trailing-whitespace)


; Enable the semantic helpers
(semantic-mode 1)
(require 'semantic/ia)
(require 'semantic/bovine/gcc)
(semantic-add-system-include "/home/user/src/lunabotautonomyd/trunk/build/include" 'c++-mode)
(global-semantic-decoration-mode 1)


; Display ANSI colors
(require 'ansi-color)
(defun colorize-compilation-buffer ()
  (toggle-read-only)
  (ansi-color-apply-on-region (point-min) (point-max))
  (toggle-read-only))
(add-hook 'compilation-filter-hook 'colorize-compilation-buffer)
(add-hook 'shell-mode-hook 'colorize-compilation-buffer)


; Additional modes
(require 'cmake-mode)


(custom-set-variables
 ;; custom-set-variables was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(c-default-style (quote ((c-mode . "gnu") (c++-mode . "gnu") (java-mode . "java") (awk-mode . "awk") (other . "gnu"))))
 '(c-doc-comment-style (quote ((c-mode . javadoc) (c++-mode . javadoc) (java-mode . javadoc) (pike-mode . autodoc))))
 '(c-offsets-alist (quote ((case-label . +))))
 '(c-tab-always-indent nil)
 '(column-number-mode t)
 '(company-auto-complete nil)
 '(company-auto-complete-chars (quote (32 40 41 46)))
 '(company-backends (quote (company-semantic company-files company-keywords company-cmake)))
 '(company-idle-delay 0.1)
 '(company-minimum-prefix-length 1)
 '(company-show-numbers t)
 '(compilation-scroll-output t)
 '(compile-command "cd admin/ && make")
 '(custom-enabled-themes (quote (wombat)))
 '(electric-indent-mode t)
 '(electric-layout-mode t)
 '(electric-pair-mode t)
 '(global-company-mode t)
 '(global-hl-line-mode t)
 '(global-hl-line-sticky-flag nil)
 '(global-linum-mode t)
 '(global-semantic-idle-summary-mode t)
 '(ido-mode (quote both) nil (ido))
 '(indent-tabs-mode nil)
 '(inhibit-startup-screen t)
 '(initial-frame-alist (quote ((fullscreen . maximized))))
 '(minimap-major-modes (quote (prog-mode)))
 '(minimap-mode nil)
 '(org-log-done (quote time))
 '(org-todo-interpretation (quote sequence))
 '(org-todo-keyword-faces (quote (("TODO" . "red") ("DONE" . "green") ("CANCELED" . "yellow"))))
 '(org-todo-keywords (quote ((sequence "TODO" "|" "DONE" "CANCELED"))))
 '(semanticdb-find-default-throttle (quote (local project unloaded system)))
 '(semanticdb-project-roots (quote ("/home/user/src/lunabotautonomyd/trunk/")))
 '(yas-global-mode t nil (yasnippet))
 '(yas-next-field-key (quote ("M-[")))
 '(yas-prev-field-key (quote ("M-]")))
 '(yas-snippet-dirs (quote ("~/.emacs.d/plugins/yasnippet/snippets" "/home/user/.emacs.d/elpa/yasnippet-0.8.0/snippets")) nil (yasnippet))
 '(yas-trigger-key "<C-tab>"))
(custom-set-faces
 ;; custom-set-faces was added by Custom.
 ;; If you edit it by hand, you could mess it up, so be careful.
 ;; Your init file should contain only one such instance.
 ;; If there is more than one, they won't work right.
 '(default ((t (:inherit nil :stipple nil :inverse-video nil :box nil :strike-through nil :overline nil :underline nil :slant normal :weight normal :height 80 :width normal :foundry "unknown" :family "Monospace"))))
 '(company-preview ((t (:background "gray9" :foreground "deep pink"))))
 '(company-preview-common ((t (:inherit company-preview))))
 '(company-preview-search ((t (:inherit company-preview))))
 '(company-scrollbar-bg ((t (:inherit company-tooltip :background "dark slate gray"))))
 '(company-scrollbar-fg ((t (:background "dark cyan"))))
 '(company-tooltip ((t (:background "gray9" :foreground "forest green"))))
 '(company-tooltip-common ((t (:inherit company-tooltip :foreground "lime green"))))
 '(company-tooltip-common-selection ((t (:inherit company-tooltip-selection))))
 '(company-tooltip-selection ((t (:inherit company-tooltip :background "gray18"))))
 '(cursor ((t (:background "red"))))
 '(font-lock-type-face ((t (:foreground "olive drab" :weight normal))))
 '(highlight ((t (:background "#454545" :foreground "#ffffff" :underline nil)))))
(put 'dired-find-alternate-file 'disabled nil)


; Highlight lines
(set-face-background hl-line-face "black")
(set-face-foreground 'highlight nil)


; Highlight tab characters
(require 'whitespace)
(setq whitespace-style '(tabs tab-mark))
(global-whitespace-mode 1)


; Mark column 80
(require 'column-marker)
(add-hook 'c-mode-common-hook (lambda () (interactive) (column-marker-1 80)))
