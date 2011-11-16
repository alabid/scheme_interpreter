(define append
  (lambda (lst1 lst2)
    (if (null? lst1) lst2 (cons (car lst1) (append (cdr lst1) lst2)))))

(define zero?
  (lambda (n)
    (= 0 n)))

(define list
  (lambda (x)
  (cons x null)))

(define cadr
  (lambda (lst)
    (car (cdr lst))))

(define cddr
  (lambda (lst)
    (cdr (cdr lst))))

(define reverse
  (lambda (lst)
  (if (null? lst) null
      (append (reverse (cdr lst)) (list (car lst))))))

(define length
  (lambda (lst)
    (if (null? lst) 0
      (+ 1 (length (cdr lst))))))

(define abs
  (lambda (n)
    (if (< n 0) (* -1 n) n)))
