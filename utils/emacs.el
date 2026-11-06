(c-add-style "reone"
             '("gnu"
	       (fill-column . 80)
	       (c++-indent-level . 4)
	       (c-basic-offset . 4)
	       (indent-tabs-mode . nil)
	       (c-offsets-alist . ((arglist-intro . ++)
				   (innamespace . 0)
				   (member-init-intro . ++)))))

(add-hook 'c-mode-common-hook
	  (function
	   (lambda nil
	     (if (string-match "reone" buffer-file-name)
		 (progn
		   (c-set-style "reone"))))))
