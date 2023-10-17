import java.util.Arrays;
import java.util.Random;
import java.util.Scanner;
import java.util.stream.Collectors;

public class Lab1 {
    static Scanner scanner = new Scanner(System.in);

    public static void main(String[] args) {
        task8();
    }

    public static String readLine(String prompt) {
        System.out.println(prompt);
        return scanner.nextLine();
    }

    public static Integer readInteger(String prompt) {
        System.out.print(prompt);
        return scanner.nextInt();
    }

    public static void task1() {
        for (int i = 0; i <= 100; i++) {
            if (i % 3 == 0 && i % 5 == 0)
                System.out.println("xyxy");
            else if (i % 3 == 0)
                System.out.println("xxx");
            else if (i % 5 == 0)
                System.out.println("yyy");
            else
                System.out.println(i);
        }
    }

    public static void task2() {
        System.out.println("Guess a number");

        int numberToGuess = (int) (Math.random() * 100) + 1;
        int numberOfAttempts = 0;

        System.out.println(numberToGuess);

        while (true) {
            int guessedNumber = readInteger("Guess a number: ");

            if (guessedNumber == numberToGuess) {
                System.out.println("Yaaay, you won!");
                System.out.println("Attempts: " + numberOfAttempts);
                break;
            }
            numberOfAttempts++;
        }
    }

    public static void task3() {
        var randomNumbers = new Random()
                .ints(11, 0, 100)
                .boxed()
                .toList();

        System.out.println("Numbers: " + randomNumbers);
        var randomNumbersSorted = randomNumbers.stream()
                .sorted()
                .toList()
                .reversed();
        System.out.println("Max: " + randomNumbersSorted.get(0));
        System.out.println("Second to max: " + randomNumbersSorted.get(1));
    }

    public static void task4() {
        var input = readLine("Provide me a line:");
        var reversedInput = new StringBuilder(input).reverse().toString();

        if (input.equals(reversedInput))
            System.out.println("Given string is a palindrome.");
        else
            System.out.println("Given string is not a palindrome.");
    }

    public static void task5() {
        String input = readLine("Provide a sentence: ");

        Arrays.stream(input.replaceAll("[.,]", "").split(" "))
                .collect(Collectors.groupingBy(String::toLowerCase, Collectors.counting()))
                .forEach((word, count) -> System.out.println("Word: " + word + ", occurrences: " + count));
    }

    public static void task6() {
        Integer input = readInteger("Provide a number: ");

        if (task6IsPrime(input))
            System.out.println("Number " + input + " is a prime number.");
        else
            System.out.println("Number " + input + " is not a prime number.");
    }

    private static boolean task6IsPrime(int number) {
        if (number != 2 && number % 2 == 0)
            return false;
        if (number != 3 && number % 3 == 0)
            return false;
        if (number != 5 && number % 5 == 0)
            return false;
        if (number != 7 && number % 7 == 0)
            return false;

        for (int i = 11; i < Math.sqrt(number) + 1; i++) {
            if (!task6IsPrime(i))
                return false;
        }

        return true;
    }

    public static void task7() {
        int height = readInteger("Provide pyramid height: ");

        for (int i = 1; i <= height; i++) {
            for (int j = 1; j <= height - i; j++)
                System.out.print(" ");
            for (int k = 1; k <= 2 * i - 1; k++)
                System.out.print("*");
            System.out.println();
        }
    }

    public static void task8() {
        double distanceInKilometers = readInteger("Provide distance in kilometers: ");

        double[] convertedDistances = task8ConvertDistance(distanceInKilometers);

        System.out.println("Distance in land miles: " + convertedDistances[0] + " miles");
        System.out.println("Distance in sea miles: " + convertedDistances[1] + " sea miles");
    }

    public static double[] task8ConvertDistance(double kilometers) {
        double landMiles = kilometers * 0.621371;
        double nauticalMiles = kilometers * 0.539957;

        return new double[]{landMiles, nauticalMiles};
    }
}
