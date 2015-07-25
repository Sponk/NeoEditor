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

				 (require 'cedet)
				 (global-ede-mode 1)

				 ;;(semantic-load-enable-excessive-code-helpers)
				 
				 (global-semantic-idle-scheduler-mode)
				 (global-semantic-idle-completions-mode)
				 (global-semantic-decoration-mode)
				 (global-semantic-highlight-func-mode)
				 (global-semantic-show-unmatched-syntax-mode)

				 (setq debug-on-error t)
				 (setq project-directory (file-name-directory
										  (let ((d (dir-locals-find-file ".")))
											(if (stringp d) d (car d)))))
				 
				 (setq project-file (concat  project-directory "CMakeLists.txt"))

				 (message project-file)
				 
				 (ede-cpp-root-project "Neo"
				                 :name "Neo"
				                 :file project-file
								 :include-path '(
				 								"./SDK/NeoCore/Includes"
				 								"./SDK/NeoEngine/Includes"
				 								"./SDK/Neo2D/include"
				 								)
				               :system-include-path '("/usr/include")
							   )
				 
				 (setq-default indent-tabs-mode t)))))) 
 )


 
