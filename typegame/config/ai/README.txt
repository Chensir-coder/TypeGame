AI reward word HTTP profile (embedded in Qt resources)
======================================================

The runtime HTTP template is bundled as :/config/ai/ai_word_request.json via
src/app/resources/typegame_assets.qrc (source file typegame/config/ai/ai_word_request.json).

1. Edit the JSON under typegame/config/ai/ai_word_request.json in the repo, then rebuild —
   Qt will embed it into typegame.exe. No separate copy next to the exe is used.

2. API key — process environment ARK_API_KEY only (templates use "Bearer ${ARK_API_KEY}").
   Set it in your IDE run configuration or shell before launching the game.

3. Do not set "stream" to true; the client only parses non-streaming responses for words.

4. Placeholders filled at runtime by AiWordService:
   - {{SYSTEM_PROMPT}}  — fixed English instructions for one-word output
   - {{USER_PROMPT}}    — topic, length, exclusion list, etc.

5. responseKind:
   - "chat_completions" — OpenAI-style `choices[0].message.content`
   - "responses"        — aggregates `output[].content[].text` from /api/v3/responses JSON

If the embedded resource fails to load or parse, Authorization is invalid, Bearer is empty after
env expand, or the HTTP request fails, the game falls back to the built-in local word list.
