(begin
    (define make-point (lambda (a b) (set-property "size" (0) (set-property "object-name" "point" (list a b)))))
    (define make-line (lambda (a b) (set-property "thickness" (1) (set-property "object-name" "line" (list a b)))))
    (define make-text (lambda (s) (set-property "position" (make-point 0 0) (set-property "object-name" "text" (s)))))
)
