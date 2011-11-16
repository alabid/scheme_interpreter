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

  (define range
    (lambda (n)
      (if (zero? n) (quote ()) 
                    (append (range (- n 1)) (list n)))))


(define ls
(lambda (x y)
(append (list x) (list y))))

(define make-queen
(lambda (row col)
(ls row col)))

(define get-row
(lambda (queen)
(car queen)))

(define get-col
(lambda (queen)
(cadr queen)))

(define same-row?
(lambda (nq oq)
(= (get-row nq) (get-row oq))))

(define same-col?
(lambda (nq oq)
(= (get-col nq) (get-col oq))))

(define same-diag?
(lambda (nq oq)
(= (abs (- (get-row nq) (get-row oq)))
(abs (- (get-col nq) (get-col oq))))))

(define attacks? 
(lambda (nq oq)
(or (same-row? nq oq) (same-col? nq oq) (same-diag? nq oq))))

(define safe?
(lambda (target queens)
(cond ((null? queens) #t)
((attacks? target (car queens)) #f)
(else (safe? target (cdr queens))))))


; Solve for a board size of sz.
(define solve
(lambda (sz)
(define s-rec
(lambda (sz x y pos sols)
(cond 
; If we've advanced past the last column, we have a solution.
; (By the way, the reverse is because pos is built up backward.)
((> x sz) (cons (reverse pos) sols))
; If we've advanced past the last row, we have a failure.
((> y sz) sols)
; If the queen is safe, the fun begins.
((safe? (make-queen x y) pos)
; This is the backtracking call. This is executed once
; the inner call is complete.
(s-rec sz x (+ y 1) pos
; Run the next column first; if any solutions
; result, they need to be passed to the backtracked
; call.
(s-rec sz (+ x 1) 1
; Add this queen when considering the next
; column's placement.
(cons (make-queen x y) pos)
sols)))
; If this queen isn't safe, move on to the next row.
(else (s-rec sz x (+ y 1) pos sols)))))
; Start the recursion.
(s-rec sz 1 1 null null)))

(define show-queens
(lambda (n)
(length (solve n))))