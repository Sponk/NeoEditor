;; Some Neo specific Emacs configs

((c-mode
  (indent-tabs-mode . t)
  (tab-width . 4)
  (c-basic-offset . 4)
  (c-file-style . "bsd"))

 (c++-mode
  (indent-tabs-mode . t)
  (tab-width . 4)
  (c-basic-offset . 4)
  (c-file-style . "bsd"))

 (nil . ((eval .
			   (progn

				 (setq project-directory (file-name-directory
										  (let ((d (dir-locals-find-file ".")))
											(if (stringp d) d (car d)))))


				 (setq cmake-ide-build-dir (concat project-directory "build"))
				 (setq cmake-ide-project-dir project-directory)
				 (setq cmake-compile-command "make -j8")
				 (setenv "MAKEFLAGS" "-j5")
				 (setq-default indent-tabs-mode t))))))


 
