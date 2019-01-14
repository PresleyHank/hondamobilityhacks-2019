# .NET Core C# Code Example

This covers basic AWS DynamoDB and S3 interactions using .NET Core. It also applies to
any later versions of .NET that support `async`/`await`. .NET Core _requires_ 
`async`/`await` use or synchronous `.Wait()`. Standard .NET can use the
synchronous API for everything without any type of waits, though it's
recommended to use asynchronous.

**The goal of this guide is to provide enough information for you to access the
data without having to spend time on a deep-dive of the official documentation.**

All .NET DynamoDB and S3 sample code and documentation is in their respective folder.
