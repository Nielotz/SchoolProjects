package io.github.nielotz.storageutilitybackend.exception.handler;

import io.github.nielotz.storageutilitybackend.exception.FoundException;
import io.github.nielotz.storageutilitybackend.exception.InvalidItemNameException;
import io.github.nielotz.storageutilitybackend.exception.NotFoundException;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.ControllerAdvice;
import org.springframework.web.bind.annotation.ExceptionHandler;
import org.springframework.web.servlet.mvc.method.annotation.ResponseEntityExceptionHandler;

@ControllerAdvice
public class ErrorHandler extends ResponseEntityExceptionHandler {
    @ExceptionHandler({NotFoundException.class})
    public ResponseEntity<String> handleNotFoundException(NotFoundException notFoundException) {
        return ResponseEntity
                .status(HttpStatus.NOT_FOUND)
                .body(notFoundException.getMessage());
    }

    @ExceptionHandler({FoundException.class})
    public ResponseEntity<String> handleConflictException(FoundException foundException) {
        return ResponseEntity
                .status(HttpStatus.CONFLICT)
                .body(foundException.getMessage());
    }

    @ExceptionHandler({InvalidItemNameException.class})
    public ResponseEntity<String> handleConflictException(InvalidItemNameException invalidItemNameException) {
        return ResponseEntity
                .status(HttpStatus.BAD_REQUEST)
                .body(invalidItemNameException.getMessage());
    }

}
