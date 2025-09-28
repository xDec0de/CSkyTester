# CSkyTester

CSkyTester, or CST for short, is a flexible test suite for C projects.
It allows you to create tests within seconds and runs them in milliseconds.

CST has been designed with performance and ease of use in mind, and while it
offers built-in tools like crash detection, it is by no means a replacement
to other tools such as valgrind or ASan. CST's goal is to provide tools to
quickly test your project **without any dependencies**.

# Main features

Even if CST isn't a replacement for other tools, it still comes with some
nice to have features. Some of which can be disabled if you don't want them,
mainly those that **may** cause issues with other solutions.

## Test registration

Tests can be easily registered with the `TEST` macro included in `cst.h`.
The macro takes two arguments:

- **Category**: The category of the tests, as tests can be grouped into
  categories. This is an optional argument, you can use `NULL` to use
  no category
- **Name**: This is the test name itself. CST doesn't require you to create
  functions for tests, but adding a name for your tests is highly recommended.
  You can of course use `NULL` if you don't want to name your test.

## Assertions

Obviously, CST comes with a lot of assertions to test your project. Assertions
are once again included in `cst.h`. Failing an assertion will fail the test it
is in. You can customize CST's behaviour if an assertion fails with these global
variables:

- **CST_SHOW_FAIL_DETAILS**: Whether to show default details about why the assertion
  failed or not. Enabled by default.
- **CST_FAIL_TIP**: Additional tip to show if an assertion fails. By default, it is
  set to `NULL` (No tip). Resets to `NULL` when any assertion occurs.

## Crash detection

CST comes with a built-in test crash detection system. It listens to known
signals that are thrown when a program crashes, intercepts them, and gives
you some details about it.

**How to disable**: `-nosig` or `-nosignal` flag.

### Planned features

- **Docs**: Docs for CST will be available as soon as CST is considered stable!
- **Memory leak detection**: I already have a working-ish version of this, but
  it requires the user to compile with some specific flags, which I don't like.
  If I come up with a solution for this, it will be implemented.
- **Traces for crash detection**: Currently, CST only says "Oh, your test has
  crashed" with a smile and lets you figure out why, not ideal.
- **Test fixtures**: Such as AFTER_EACH, BEFORE_EACH and so on.
